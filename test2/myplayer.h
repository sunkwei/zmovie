#ifndef MYPLAYER_H
#define MYPLAYER_H

#include <QObject>
#include <QQuickPaintedItem>
#include <QTimer>
#include <QAudioOutput>
#include <QIODevice>
#include <assert.h>
#include "mediathread.h"
#include "circ_buf.h"
#include "kvconfig2.h"
#include "detect/DetectLoader.h"
#include <QStack>
#include <math.h>

extern KVConfig2 *_cfg;

#define AU_BUFSIZE (64*1024)
class AudioBuffer : public QIODevice
{
    bool first_audio_;  // 一开始，等待缓冲的数据足够多时，再开始填充 ...
    MediaThread *mt_;
    double stamp_audio_delta_;
    char buf_[AU_BUFSIZE];
    int head_, tail_;

public:
    AudioBuffer(MediaThread *mt);

    int idle_size() const { return CIRC_SPACE(head_, tail_, AU_BUFSIZE); }
    int data_size() const { return CIRC_CNT(head_, tail_, AU_BUFSIZE); }

private:
    void append(unsigned char *data, int len)
    {
        assert(idle_size() >= len);

        int se = CIRC_SPACE_TO_END(head_, tail_, AU_BUFSIZE);
        if (se >= len) {
            memcpy(buf_+head_, data, len);
        }
        else {
            memcpy(buf_+head_, data, se);
            memcpy(buf_, data+se, len-se);
        }

        head_ = (head_ + len) & (AU_BUFSIZE-1);
    }

    // 尽量填充本地 buf
    void load_from_mt()
    {
        while (mt_->audio_pending_size() > 0) {
            int bytes = mt_->audio_pending_next_bytes();
            if (idle_size() >= bytes) {
                MediaThread::Pcm *pcm = mt_->lock_pcm();
                int len;
                void *p = pcm->data(len);
                assert(len == bytes);
                append((unsigned char*)p, len);
                mt_->unlock_pcm(pcm);
            }
            else {
                break;
            }
        }
    }

    virtual qint64 readData(char *data, qint64 maxlen);
    virtual qint64 writeData(const char *data, qint64 len);
};

class MyPlayer : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QString url READ url WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(bool cl_enabled READ cl_enabled WRITE cl_setEnabled NOTIFY cl_enabledChanged)
    Q_PROPERTY(double cache_duration READ cache_duration WRITE setCache_duration NOTIFY cache_durationChanged)
    Q_PROPERTY(bool det_enabled READ det_enabled WRITE det_setEnabled NOTIFY det_enabledChanged)
    Q_PROPERTY(QString cl_points_desc READ cl_points_desc WRITE setCl_points_desc NOTIFY cl_points_descChanged)
    Q_PROPERTY(bool rt_enabled READ rt_enabled WRITE setRt_enabled NOTIFY rt_enabledChanged)


public:
    MyPlayer();
    virtual ~MyPlayer();
    virtual void paint(QPainter *painter);

    QString url() const { return url_; }
    void setUrl(const QString &url) { url_ = url; }

    bool cl_enabled() const { return cl_enabled_; }
    void cl_setEnabled(bool enable)
    {
        cl_enabled_ = enable;
    }

    QString cl_points_desc() const;
    void setCl_points_desc(const QString &desc);

    bool det_enabled() const { return det_enabled_; }
    void det_setEnabled(bool e);

    double cache_duration() const { return duration_; }
    void setCache_duration(double d) { duration_ = d; }

    bool rt_enabled() const { return rt_enabled_; }
    void setRt_enabled(bool enabled)
    {
        rt_enabled_ = enabled;
    }

    /** 左键按住，拖动 ...
     */
    Q_INVOKABLE void rt_tracking(const QPoint &p1, const QPoint &p2)
    {
        if (rt_enabled_) {
            rt_p1_ = p1;
            rt_p2_ = p2;
        }
    }

    /** 左键释放，如果 p1,p2 很近，则认为是尝试选中某个框 ...
     */
    Q_INVOKABLE void rt_tracked(const QPoint &p1, const QPoint &p2)
    {
        if (!rt_enabled_) {
            return;
        }

        float dx = p1.x() - p2.x(), dy = p1.y() - p2.y();
        if (sqrt(dx*dx + dy*dy) < 3.0) {
            // 算作选中吧 ...
        }
        else {
            rt_p1_ = p1, rt_p2_ = p2;
        }
    }

    /** 右键释放，则删除 ...
     */
    Q_INVOKABLE void rt_clear()
    {
        if (!rt_enabled_) {
            return;
        }

        rt_p1_ = QPoint(-1, -1);
    }

    /** 返回框框 */
    Q_INVOKABLE QString rt_desc() const
    {
        if (!rt_enabled_) {
            return "";
        }
        else {
            char buf[128];
            snprintf(buf, sizeof(buf), "(%d,%d),(%d,%d)",
                     rt_p1_.x(), rt_p1_.y(),
                     rt_p2_.x(), rt_p2_.y());
            return buf;
        }
    }

    Q_INVOKABLE void play();
    Q_INVOKABLE void stop();

    Q_INVOKABLE void cl_push_point(int x, int y);
    Q_INVOKABLE void cl_pop_point();
    Q_INVOKABLE void cl_remove_all_points();

    Q_INVOKABLE void det_set_params(double thres_dis, double thres_area, double factor_0, double factor_05);

    /*  新增对象，至少包含:
     *      type: ["line", "circle" ]
     *
     *  TODO: 应该增加颜色，模板 ....
    */
    Q_INVOKABLE void dr_push(const QVariantMap &draw_desc); // 新增一个画图对象 ..
    Q_INVOKABLE void dr_pop();
    Q_INVOKABLE void dr_clear();

private slots:
    void when_check_frame();
    void when_cl_enabledChanged();
    void when_cache_durationChanged();
    void when_det_enabledChanged(bool);

signals:
    void urlChanged();
    void cl_enabledChanged();
    void cl_points_descChanged();
    void cache_durationChanged();
    void det_enabledChanged(bool);
    void rt_enabledChanged();

private:
    void check_video_frame(double now);
    void load_calibration_data(const QString &desc);
    void draw_cl_lines(QImage *img);
    void draw_det_result(const std::vector<QRect> &rcs, QImage *image);
    void draw_dr_history(QImage *image);
    void draw_dr_one(QPainter &p, const QVariantMap &oper);
    void draw_rt(QImage *img);

private:
    QString url_, info_;
    QTimer timer_;
    MediaThread *th_;
    MediaThread::Picture *img_rending_;
    bool first_video_, first_audio_;
    double stamp_video_delta_, stamp_audio_delta_;
    QAudioOutput *audio_output_;
    AudioBuffer *ab_;

    bool cl_enabled_;
    std::deque<QPoint> cl_points_;

    bool rt_enabled_;   // 是否允许鼠标画橡皮筋 ...
    QPoint rt_p1_, rt_p2_;  // 需要画的矩形的两个点 ...

    double duration_;

    bool det_enabled_;
    DetectLoader *detect_loader_;

    KVConfig2 *cfg_;

    QStack<QVariantMap> dr_opers;   // 需要画出来的历史命令 ...
};

#endif // MYPLAYER_H
