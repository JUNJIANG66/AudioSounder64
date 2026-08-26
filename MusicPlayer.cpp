#include "MusicPlayer.h"
#include<QIcon.h>
#include<QSize.h>
#include<QPushButton.h>
#include<QPixmap.h>
#include<QPalette.h>
#include <QDir.h>
#include <QListWidgetItem>
#include<QMessageBox>
#include<QVariant>
#include <QRandomGenerator>
#include <QFileDialog>
#include <QSettings>
MusicPlayer::MusicPlayer(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    setWindowTitle("AudioSounder64");
    setWindowIcon(QIcon(":/icon/icon/musicPlayer.png"));
    setFixedSize(930, 620);

    setBackground(":/img/img/background.jpg");

    intimenu();

    intiwidget();
  
    
     

    
}

MusicPlayer::~MusicPlayer()
{}

void MusicPlayer::setButtonStyle(QPushButton * btn, const QString & filepath,const QSize& size)
{
    btn->setFixedSize(size);
    btn->setIcon(QIcon(filepath));
    btn->setIconSize(QSize(btn->width(), btn->height()));
    btn->setStyleSheet("background-color:transparent");
}

void MusicPlayer::intiwidget()
{
    
    ui.menuBar->setNativeMenuBar(false); //必须关闭原生菜单栏，样式才生效
    ui.menuBar->setStyleSheet("QMenuBar{ background-color:black; color:white; }");

    setButtonStyle(ui.mode, ":/icon/icon/sequence.png", QSize(50, 50));
    setButtonStyle(ui.previous, ":/icon/icon/previous.png", QSize(50, 50));
    setButtonStyle(ui.ifpause, ":/icon/icon/player.png", QSize(60, 60));
    setButtonStyle(ui.next, ":/icon/icon/next.png", QSize(50, 50));
    setButtonStyle(ui.playlist, ":/icon/icon/playlist.png", QSize(50, 50));

    intimedia();
    intimusicList(getDefaultPath());
    

    connect(ui.ifpause, &QPushButton::clicked, this, [&]() 
        {
            musicManager(currentMusic);
        
        });

    ui.musiclist->hide();
    connect(ui.playlist, &QPushButton::clicked, this, [&]()
        {
            musicListManager();
        });
    connect(ui.musiclist, &QListWidget::itemClicked, this, &MusicPlayer::clickListItem);
    connect(ui.musiclist, &QListWidget::itemClicked, this, [&]()
        {
            musicManager(currentMusic);

        });
    connect(ui.previous, &QPushButton::clicked, this, [&]()
        {
            switch (currentMode)
            {
            case sequence:preplay(); break;
            case random:
            {
                if (currentMusic == "nothing")
                {
                    QMessageBox::warning(this, "提示", "当前文件夹没有可以播放的音乐");
                    break;
                }
                int pick;
                do {
                    pick = QRandomGenerator::global()->bounded(0, musicMaxId + 1);
                } while (pick == currentID && musicMaxId > 0);   // 多于一首时才要求不重复
                currentID = pick;
                ui.musiclist->setCurrentRow(currentID);
                emit ui.musiclist->itemClicked(ui.musiclist->currentItem());
                break;
            }
            case singleLoop:
                if (currentMusic == "nothing")
                {
                    QMessageBox::warning(this, "提示", "当前文件夹没有可以播放的音乐");
                    break;
                }
                clickListItem();
                musicManager(currentMusic);
                break;
            }
        });
    connect(ui.next, &QPushButton::clicked, this, [&]()

        {
            switch (currentMode)
            {
            case sequence:nextplay(); break;
            case random:
            {
                if (currentMusic == "nothing")
                {
                    QMessageBox::warning(this, "提示", "当前文件夹没有可以播放的音乐");
                    break;
                }
                int pick;
                do {
                    pick = QRandomGenerator::global()->bounded(0, musicMaxId + 1);
                } while (pick == currentID && musicMaxId > 0);   // 多于一首时才要求不重复
                currentID = pick;
                ui.musiclist->setCurrentRow(currentID);
                emit ui.musiclist->itemClicked(ui.musiclist->currentItem());
                break;
            }
            case singleLoop:
                if (currentMusic == "nothing")
                {
                    QMessageBox::warning(this, "提示", "当前文件夹没有可以播放的音乐");
                    break;
                }
                clickListItem();
                musicManager(currentMusic);
                break;
    }
        });
    connect(ui.mode, &QPushButton::clicked, this, &MusicPlayer::changeMode);
    connect(m_player, &QMediaPlayer::positionChanged, this, &MusicPlayer::updateSlider);
    connect(ui.musicSlider, &QSlider::sliderReleased, this, &MusicPlayer::rollSlider);
}



void MusicPlayer::setBackground(const QString& filepath)
{
    QPixmap img(filepath);
    QSize windowSize = this->size();
    QPixmap scaleimg = img.scaled(windowSize, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

    QPalette palette = this->palette();
    palette.setBrush(QPalette::Window, QBrush(scaleimg));
    this->setPalette(palette);


}

void MusicPlayer::intimedia()
{
    if (!m_player)//初始化一次媒体
    {
        m_player = new QMediaPlayer(this);
        a_output = new QAudioOutput(this);
        a_output->setVolume(0.1);
        m_player->setAudioOutput(a_output);
        connect(m_player, &QMediaPlayer::mediaStatusChanged, this,
            [this](QMediaPlayer::MediaStatus status) {
                if (status == QMediaPlayer::EndOfMedia)
                {
                    qDebug() << "播完了";
                    emit ui.next->clicked();
                }

            });
    }
}

void MusicPlayer::musicManager(const QString& filepath)
{
    if (filepath == "nothing")
    {
        QMessageBox::warning(this, "提示", "当前文件夹没有可以播放的音乐");
        return;
    }
    if (m_player->playbackState() == QMediaPlayer::StoppedState)
    {
        m_player->setSource(QUrl::fromLocalFile(filepath));
        m_player->play();
        setButtonStyle(ui.ifpause, ":/icon/icon/pause.png", QSize(60, 60));
    }
        
    else if(m_player->playbackState() == QMediaPlayer::PausedState)
        {
            m_player->play();
            setButtonStyle(ui.ifpause, ":/icon/icon/pause.png", QSize(60, 60));
        }
       
    else
    {
        m_player->pause();
        setButtonStyle(ui.ifpause, ":/icon/icon/player.png", QSize(60, 60));
    }
   
}



void MusicPlayer::intisilder()
{  
    ui.musicSlider->setRange(0, m_player->duration());
}

void MusicPlayer::updateSlider()
{
    intisilder();
    if(!ui.musicSlider->isSliderDown())
    ui.musicSlider->setValue(m_player->position());

}

void MusicPlayer::rollSlider()
{
    m_player->setPosition(ui.musicSlider->value());
    m_player->play();
    setButtonStyle(ui.ifpause, ":/icon/icon/pause.png", QSize(60, 60));

}

void MusicPlayer::intimusicList(const QString& filepath)
{
    
    ui.musiclist->clear();

    QDir dir(filepath);//创建文件夹对象

    QStringList audioFilters{//设置通配符
 "*.mp3", "*.wav", "*.flac",
 "*.aac", "*.m4a", "*.ogg", "*.wma"
    };
    dir.setNameFilters(audioFilters);//设置文件名筛选
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);//设置文件类型筛选
    if (dir.count() == 0)
    {
        currentMusic = "nothing";
        musicMaxId = 0;
        currentID = 0;
        return;
    }
    else 
    {
        QFileInfoList fileInfos = dir.entryInfoList();//获取文件信息列表

        for (const QFileInfo& file : fileInfos)//遍历其中的文件信息
        {

            QString displayName = file.fileName();

            QListWidgetItem* item = new QListWidgetItem(displayName);//创建文件名列表项

            setMusicPath(file, item);//保存音乐文件路径到虚拟自定义位置

            ui.musiclist->addItem(item);//加入列表项
        }
        ui.musiclist->setCurrentRow(0);
        currentID = 0;
        QVariant musicpath = ui.musiclist->currentItem()->data(Qt::UserRole);
        currentMusic = musicpath.toString();
        musicMaxId = ui.musiclist->count()-1;
    }

}

void MusicPlayer::musicListManager()
{
  
    if (!ui.musiclist->isVisible())
    {
        ui.musiclist->show();
    }
    else
    {
        ui.musiclist->hide();
    }

}

void MusicPlayer::clickListItem()
{
   QVariant musicpath = ui.musiclist->currentItem()->data(Qt::UserRole);
   currentMusic = musicpath.toString();
   
   currentID = ui.musiclist->currentRow();

   m_player->stop();
   m_player->setSource(QUrl());


}

void MusicPlayer::setMusicPath(const QFileInfo& file, QListWidgetItem* item)
{
    item->setData(Qt::UserRole, file.absoluteFilePath());

}

void MusicPlayer::nextplay()
{
    if (currentMusic == "nothing")
    {
        QMessageBox::warning(this, "提示", "当前文件夹没有可以播放的音乐");
        return;
    }
    else if (currentID < musicMaxId)
    {
        currentID++;
        ui.musiclist->setCurrentRow(currentID);
        emit ui.musiclist->itemClicked(ui.musiclist->currentItem());
    }
    else
    {
        currentID = 0;
        ui.musiclist->setCurrentRow(currentID);
        emit ui.musiclist->itemClicked(ui.musiclist->currentItem());
    }
}

void MusicPlayer::preplay()
{
    if (currentMusic == "nothing")
    {
        QMessageBox::warning(this, "提示", "当前文件夹没有可以播放的音乐");
        return;
    }
    else if (currentID >0)
    {
        currentID--;
        ui.musiclist->setCurrentRow(currentID);
        emit ui.musiclist->itemClicked(ui.musiclist->currentItem());
    }
    else
    {
        currentID = musicMaxId;
        ui.musiclist->setCurrentRow(currentID);
        emit ui.musiclist->itemClicked(ui.musiclist->currentItem());
    }
}

void MusicPlayer::changeMode()
{
    switch (currentMode)
    {
    case sequence:
        setButtonStyle(ui.mode, ":/icon/icon/random.png", QSize(50, 50));
        currentMode = random;
        break;
    case random:
        setButtonStyle(ui.mode, ":/icon/icon/singleLoop.png", QSize(50, 50));
        currentMode = singleLoop;
        break;
    case singleLoop:
        setButtonStyle(ui.mode, ":/icon/icon/sequence.png", QSize(50, 50));
        currentMode = sequence;
        break;
    
    }
}

void MusicPlayer::intimenu()
{
    connect(ui.addMusicFiles, &QAction::triggered, this, &MusicPlayer::addFiles);
}

void MusicPlayer::addFiles()
{
    currectPath= QFileDialog::getExistingDirectory(this,"请选择音频文件夹","", QFileDialog::ShowDirsOnly);
    if (currectPath.isEmpty())
        return;
    intimusicList(currectPath);
    if(!ui.musiclist->isVisible())
        ui.musiclist->show();
    m_player->stop();
    m_player->setSource(QUrl());
    setButtonStyle(ui.ifpause, ":/icon/icon/player.png", QSize(60, 60));
    sustainDefaultPath();

}

void MusicPlayer::sustainDefaultPath()
{
    QSettings sustain("jjh", "AudioSounder64");
    sustain.setValue("defaultPath", currectPath);

}

QString MusicPlayer::getDefaultPath()
{
    QSettings sustain("jjh", "AudioSounder64");
    QVariant path = sustain.value("defaultPath");
    return path.toString();
}











