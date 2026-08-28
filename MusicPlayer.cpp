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
#include<QAction>
#include<QLineEdit>
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
    //初始化样式
    ui.menuBar->setNativeMenuBar(false); //必须关闭原生菜单栏，样式才生效
    ui.menuBar->setStyleSheet("QMenuBar{ background-color:black; color:white; }");
    ui.musicFind->setStyleSheet("background-color:white;color:black;  border:1px solid #ccc;");
    QAction* findSign = new QAction(ui.musicFind);//设置搜索框图标和清空
    QIcon findIcon(":/icon/icon/find.png");
    findSign->setIcon(findIcon.pixmap(QSize(16,16)));
    findSign->setEnabled(false);
    ui.musicFind->addAction(findSign, QLineEdit::LeadingPosition);

    QAction* clearSign = new QAction(ui.musicFind);
    QIcon clearIcon(":/icon/icon/clear.png");
    clearSign->setIcon(clearIcon.pixmap(QSize(16, 16)));
    ui.musicFind->addAction(clearSign, QLineEdit::TrailingPosition);
    clearSign->setVisible(0);
    connect(clearSign, &QAction::triggered, ui.musicFind, &QLineEdit::clear);
    connect(ui.musicFind, &QLineEdit::textChanged, this,[=]()
        {
            if (ui.musicFind->text().isEmpty())
                clearSign->setVisible(0);
            else 
                clearSign->setVisible(1);
        });

    ui.currentTime->setStyleSheet(R"(
                    background-color:black;
                    border-radius:4px;
                    padding:2px 6px;
                                  )");//设置时间进度样式
    ui.sumTime->setStyleSheet(R"(
                    background-color:black;
                    border-radius:4px;
                    padding:2px 6px;
                                  )");
    
    //初始化按钮
    setButtonStyle(ui.mode, ":/icon/icon/sequence.png", QSize(50, 50));
    setButtonStyle(ui.previous, ":/icon/icon/previous.png", QSize(50, 50));
    setButtonStyle(ui.ifpause, ":/icon/icon/player.png", QSize(60, 60));
    setButtonStyle(ui.next, ":/icon/icon/next.png", QSize(50, 50));
    setButtonStyle(ui.playlist, ":/icon/icon/playlist.png", QSize(50, 50));
    //初始化控件功能
    intimedia();//初始化媒体
    intimusicList(getDefaultPath());//列表内容初始化
    

    connect(ui.ifpause, &QPushButton::clicked, this, [&]() 
        {
            musicManager(currentMusic);
        
        });//加载播放按钮功能

    ui.musiclist->hide();
    ui.musicFind->hide();
    connect(ui.playlist, &QPushButton::clicked, this, [&]()
        {
            musicListManager();
        });//加载显示列表按钮功能
    connect(ui.musiclist, &QListWidget::itemClicked, this, &MusicPlayer::clickListItem);//加载列表选项播放功能
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
                if (currectPath == "nothing")
                {
                    QMessageBox::warning(this, "提示", "请选择音频文件夹");
                    return;
                }
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
                if (currectPath == "nothing")
                {
                    QMessageBox::warning(this, "提示", "请选择音频文件夹");
                    return;
                }
                if (currentMusic == "nothing")
                {
                    QMessageBox::warning(this, "提示", "当前文件夹没有可以播放的音乐");
                    break;
                }
                clickListItem();
                musicManager(currentMusic);
                break;
            }
        });//加载播放上下音频按钮功能
    connect(ui.next, &QPushButton::clicked, this, [&]()

        {
            switch (currentMode)
            {
            case sequence:nextplay(); break;
            case random:
            {
                if (currectPath == "nothing")
                {
                    QMessageBox::warning(this, "提示", "请选择音频文件夹");
                    return;
                }
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
                if (currectPath == "nothing")
                {
                    QMessageBox::warning(this, "提示", "请选择音频文件夹");
                    return;
                }
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
    connect(ui.mode, &QPushButton::clicked, this, &MusicPlayer::changeMode);//加载播放模式切换功能


    ui.currentTime->setText("00:00");//加载进度条功能
    connect(m_player, &QMediaPlayer::durationChanged, this, &MusicPlayer::intisilder);
    connect(m_player, &QMediaPlayer::positionChanged, this, &MusicPlayer::updateSlider);
    connect(ui.musicSlider, &QSlider::sliderReleased, this, &MusicPlayer::rollSlider);
    connect(ui.musicSlider, &QSlider::valueChanged, this, &MusicPlayer::setCurrentTime);


    
    connect(ui.musicFind, &QLineEdit::textChanged, this, &MusicPlayer::findManager);//加载搜索框功能
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
    if (currectPath == "nothing")
    {
        QMessageBox::warning(this, "提示", "请选择音频文件夹");
        return;
    }
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
    int sum = m_player->duration();
    QString sumtime = QTime(0, sum / 1000 / 60, sum / 1000 % 60, 0).toString("mm:ss");
    ui.sumTime->setText(sumtime);
    ui.musicSlider->setRange(0, sum);



    
}

void MusicPlayer::updateSlider()
{
    
    int cur = m_player->position();
    if (!ui.musicSlider->isSliderDown())
    {
        ui.musicSlider->setValue(cur);
        QString curtime = QTime(0, cur / 1000 / 60, cur / 1000 % 60, 0).toString("mm:ss");
        ui.currentTime->setText(curtime);
    }
 
}

void MusicPlayer::rollSlider()
{
    m_player->setPosition(ui.musicSlider->value());
    m_player->play();
    setButtonStyle(ui.ifpause, ":/icon/icon/pause.png", QSize(60, 60));
    
}

void MusicPlayer::setCurrentTime()
{
    int cur = ui.musicSlider->value();
    QString curtime = QTime(0, cur / 1000 / 60, cur / 1000 % 60, 0).toString("mm:ss");
    ui.currentTime->setText(curtime);
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
        fileInfos.clear();
        currentMusic = "nothing";
        musicMaxId = 0;
        currentID = 0;
        currectPath = filepath;
        return;
    }
    else 
    {
         fileInfos = dir.entryInfoList();//获取文件信息列表

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
        currectPath = filepath;
        m_player->setSource(QUrl::fromLocalFile(currentMusic));
    }

}

void MusicPlayer::musicListManager()
{
  
    if (!ui.musiclist->isVisible())
    {
        ui.musiclist->show();
        ui.musicFind->show();
    }
    else
    {
        ui.musiclist->hide();
        ui.musicFind->hide();
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
    if (currectPath == "nothing")
    {
        QMessageBox::warning(this, "提示", "请选择音频文件夹");
        return;
    }
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
    if (currectPath == "nothing")
    {
        QMessageBox::warning(this, "提示", "请选择音频文件夹");
        return;
    }
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
    if (!ui.musiclist->isVisible())
    {
        ui.musiclist->show();
        ui.musicFind->show();
    }
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

void MusicPlayer::findManager()
{
    if(currectPath=="nothing")
    {
        if (!ui.musicFind->text().isEmpty())
        {
            QMessageBox::warning(this, "提示", "请选择音频文件夹");
            ui.musicFind->clear();
        }
        return;
    }
    if (fileInfos.count() == 0)
    {
        if (!ui.musicFind->text().isEmpty())
        {
            QMessageBox::warning(this, "提示", "当前文件夹没有可以播放的音乐");
            ui.musicFind->clear();
        }
        return;
    }
    for (int i = 0; i <= musicMaxId; i++)
    {
        ui.musiclist->item(i)->setHidden(1);
    }
    int first = currentID;//优选选中
    bool dic = 1;
    for (int i = 0; i <= musicMaxId; i++)
    {
        if (ui.musiclist->item(i)->text().contains(ui.musicFind->text(), Qt::CaseInsensitive)|| ui.musicFind->text().isEmpty())
        {
            ui.musiclist->item(i)->setHidden(0);
            if (dic&& !ui.musicFind->text().isEmpty()) { first = i; dic = 0; }
        }
    }
    ui.musiclist->setCurrentRow(first);

}











