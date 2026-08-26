#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MusicPlayer.h"
#include<QMediaPlayer.h>
#include <QAudioOutput.h>
#include <QFileInfo>
#include <QListWidgetItem>  
class MusicPlayer : public QMainWindow
{
    Q_OBJECT

public:
    MusicPlayer(QWidget *parent = nullptr);
    ~MusicPlayer();

  

private:
    Ui::MusicPlayerClass ui;//UI控制
    void setBackground(const QString& filepath);//设置窗口背景



    void setButtonStyle(QPushButton* btn, const QString& filepath, const QSize& size);//设置按钮格式

    QMediaPlayer* m_player = nullptr;//媒体
    QAudioOutput* a_output = nullptr;//音频输出
    void intimedia();//初始化媒体
    void musicManager(const QString& filepath);//音乐播放按钮管理
    

    void intimusicList(const QString& filepath);//播放列表初始化
    void musicListManager();//播放列表按钮管理
    void clickListItem();//列表项切换播放
    void setMusicPath(const QFileInfo&file, QListWidgetItem* item);//设置存储音乐的路径
    QString currentMusic="nothing";//当前选中的音乐的路径
    int currentID = 0;//当前选中的音乐的id
    int musicMaxId = 0;//音乐最大id
    void nextplay();//next按钮管理
    void preplay();//previous按钮管理
    //播放模式枚举
    enum playmode
    {
        sequence,
        random,
        singleLoop
    };
    playmode currentMode = sequence;//默认播放模式
    void changeMode();//模式切换按钮管理
 

    void intisilder();//进度条初始化
    void updateSlider();//更新进度条
    void rollSlider();//滚动进度条

    void intiwidget();//初始化控件

    void intimenu();//初始化菜单
    void addFiles();//添加文件夹
    QString currectPath="nothing";//当前文件夹路径
    void sustainDefaultPath();//存储为默认路径
    QString getDefaultPath();//获取默认路径

    
    
   
};

