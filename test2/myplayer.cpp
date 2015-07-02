#include "myplayer.h"
#include <QPainter>
#include <stdio.h>
#include <QRect>
#include <QPen>
#include <sstream>
#include <QtScript/QScriptEngine>
#include "cJSON.h"

MyPlayer::MyPlayer()
{
    cfg_ = _cfg;

    img_rending_ = 0;
    timer_.setInterval(10);
    QObject::connect(&timer_, SIGNAL(timeout()), this, SLOT(when_check_frame()));
    QObject::connect(this, SIGNAL(cl_enabledChanged()), this, SLOT(when_cl_enabledChanged()));
    QObject::connect(this, SIGNAL(det_enabledChanged(bool)), this, SLOT(when_det_enabledChanged(bool)));

    th_ = 0;

    QAudioFormat fmt;
    fmt.setChannelCount(2);
    fmt.setSampleRate(32000);
    fmt.setSampleSize(16);
    fmt.setCodec("audio/pcm");
    fmt.setByteOrder(QAudioFormat::LittleEndian);
    fmt.setSampleType(QAudioFormat::SignedInt);

    audio_output_ = new QAudioOutput(fmt, this);
    ab_ = 0;
    cl_enabled_ = false;
    det_enabled_ = false;
    detect_loader_ = 0;

    load_calibration_data();

    /** 设置默认缓冲时间 .. */
    duration_ = 0.0;
}

MyPlayer::~MyPlayer()
{
    stop();
    delete audio_output_;
}

void MyPlayer::when_check_frame()
{
    double now = util_now();
    check_video_frame(now);
}

void MyPlayer::when_cache_durationChanged()
{

}

void MyPlayer::when_det_enabledChanged(bool e)
{
}

void MyPlayer::check_video_frame(double now)
{
    // 每隔10ms，检查是否有需要 render 的图像 ...
    assert(th_);

#if 0
    if (first_video_) {
        // 等待至少缓冲几帧之后，再开始真正的播放，这样能够更平滑 ...
        if (th_->video_pending_duration() > duration_) {
            first_video_ = false;
            stamp_video_delta_ = now - th_->video_pending_first_stamp();
            qDebug("pending video duration %.3f, size=%u", th_->video_pending_duration(), th_->video_pending_size());
        }
        else {
            return;
        }
    }

    if (th_->video_pending_size() == 0) {
        // 说明似乎是播放的速度太快了，需要重新等待缓冲满???
        first_video_ = true;
        qDebug("video underflow ....");
    }
    else {
        // 检查是否已经 painter 完成 ...
        if (!img_rending_) {
            // 检查是否此时应该播放该帧 ...
            double stamp = th_->video_pending_first_stamp();
            if (now - stamp_video_delta_ > stamp) {
                MediaThread::Picture *p = th_->lock_picture();
                img_rending_ = p;
                update();
            }
        }
        else {
            qDebug("rending op tooo slow !!!");
        }
    }
#else
    // FIXME: 立即播放 ...
    if (th_->video_pending_size() > 0) {
        if (img_rending_) {
            qDebug("rending op tooo slow !!!");
        }
        else {
            MediaThread::Picture *p = th_->lock_picture();
            img_rending_ = p;
            update();
        }
    }
#endif //
}

void MyPlayer::draw_cl_lines(QImage *img)
{
    QPen pen(QColor(0, 255, 0)), pent(QColor(255, 0, 0));
    pen.setWidth(3), pent.setWidth(3);

    std::deque<QPoint>::const_iterator it = cl_points_.begin(), it0 = it;

    QPainter painter;
    QPainter *p = &painter;

    p->begin(img);

    int l = img->width(), r = 0;    // 左右边沿 ..

    int x0 = it0->x() * img->width() / width(), x = x0;
    int y0 = it0->y() * img->height() / height(), y = y0;

    l = x0 < l ? x0 : l;
    r = x0 > r ? x0 : r;

    p->setPen(pen);
    p->drawRect(x0-5, y0-5, 10, 10);

    for (++it; it != cl_points_.end(); ++it, ++it0) {
        int x = it->x() * img->width() / width();
        int y = it->y() * img->height() / height();

        l = x < l ? x : l;
        r = x > r ? x : r;

        p->drawLine(QPoint(x0, y0), QPoint(x, y));

        x0 = x, y0 = y;
    }

    if (x != x0 || y != y0) {
        p->setPen(pent);
        p->drawLine(QPoint(x, y), QPoint(x0, y0));
    }

    // 画left, right线 ..
    if (cl_points_.size() >= 2) {
        p->setPen(QPen(QColor(255, 255, 0)));
        p->drawLine(QPoint(l, 0), QPoint(l, img->height()));
        p->drawLine(QPoint(r, 0), QPoint(r, img->height()));
    }

    p->end();
}

static zifImage *build_zifImage(MediaThread::Picture *img)
{
    // 需要从 rgb32 转换为 bgr24 .
    zifImage *zimg = (zifImage*)malloc(sizeof(zifImage));
    zimg->width = img->width();
    zimg->height = img->height();
    const AVPicture &pic = img->pic();
    for (int i = 0; i < 4; i++) {
        zimg->data[i] = pic.data[i];
        zimg->stride[i] = pic.linesize[i];
    }

    return zimg;
}

static void free_zifImage(zifImage *zimg)
{
    free(zimg);
}

// 探测结果，从 det_detect() 中返回的 json 字符串中解析 ...
struct DetectResult
{
    double stamp;
    std::vector<QRect> rcs;
    int flipped_index;
};

static std::vector<QRect> _parse_rects(cJSON *j)
{
    std::vector<QRect> rcs;

    assert(j->type == cJSON_Array);
    cJSON *c = j->child;

    while (c) {
        assert(c->type == cJSON_Object);
        cJSON *r = c->child;

        QRect rc(-1, -1, 0, 0);

        while (r) {
            if (!strcmp("x", r->string))
                rc.setX(r->valueint);
            if (!strcmp("y", r->string))
                rc.setY(r->valueint);
            if (!strcmp("width", r->string))
                rc.setWidth(r->valueint);
            if (!strcmp("height", r->string))
                rc.setHeight(r->valueint);

            r = r->next;
        }

        if (rc.width() * rc.height() > 5) {
            rcs.push_back(rc);
        }

        c = c->next;
    }

    return rcs;
}

/** { "stamp": 12345,
 *    "rect": [
 *       { "x": XX, "y": YY, "width": WWW, "height": HHH},
 *       { "x": XX, "y": YY, "width": WWW, "height": HHH},
 *         ....
 *    ],
 *    "flipped_idx": INDEX
 *  }
 */
static DetectResult parse_det_result(const char *str)
{
    DetectResult dr = { -1 };
    if (str) {
        cJSON *j = cJSON_Parse(str);
        if (j) {
            assert(j->type == cJSON_Object);

            cJSON *c = j->child;
            while (c) {
                if (!strcmp("stamp", c->string)) {
                    assert(c->type == cJSON_Number);
                    dr.stamp = c->valuedouble;
                }
                if (!strcmp("rect", c->string)) {
                    assert(c->type == cJSON_Array);
                    dr.rcs = _parse_rects(c);
                }
                if (!strcmp("flipped_index", c->string)) {
                    assert(c->type == cJSON_Number);
                    dr.flipped_index = c->valueint;
                }

                c = c->next;
            }
            cJSON_Delete(j);
        }
    }

    return dr;
}

void MyPlayer::draw_det_result(const std::vector<QRect> &rcs, QImage *image)
{
    QPainter p;
    p.begin(image);
    QPen pen(QColor(255, 0, 0));
    pen.setWidth(3);
    p.setPen(pen);

    for (std::vector<QRect>::const_iterator it = rcs.begin(); it != rcs.end(); ++it) {
        p.drawRect(*it);
    }

    p.end();
}

void MyPlayer::draw_dr_one(QPainter &p, const QVariantMap &oper)
{
    QVariantMap::const_iterator itf = oper.find("name");
    if (itf == oper.end()) {
        return;
    }

    QString cmd = itf.value().toString();

    if (cmd == "line") {
        // 画线, x0,y0,  x1,y1
        if (oper.find("x0") != oper.end() && oper.find("y0") != oper.end() &&
            oper.find("y0") != oper.end() && oper.find("y1") != oper.end()) {
            int x0 = oper["x0"].toInt();
            int y0 = oper["y0"].toInt();
            int x1 = oper["x1"].toInt();
            int y1 = oper["y1"].toInt();

            QPen pen(QColor(255, 0, 0));
            p.setPen(pen);

            p.drawLine(QPoint(x0, y0), QPoint(x1, y1));
        }
    }
    else {
        // 未支持 ..
        fprintf(stderr, "MyPlayer::draw_dr_one: notimpl cmd=%s\n", cmd.toStdString().c_str());
    }
}

void MyPlayer::draw_dr_history(QImage *image)
{
    if (!dr_opers.empty()) {
        QPainter p;
        p.begin(image);

        QStack<QVariantMap>::const_iterator it;
        for (it = dr_opers.begin(); it != dr_opers.end(); ++it) {
            draw_dr_one(p, *it);
        }

        p.end();
    }
}

void MyPlayer::paint(QPainter *painter)
{
    if (img_rending_ && th_) {
        QRectF r(0, 0, width(), height());

        QImage *img = img_rending_->image();

        // 画标定线 ...
        if (cl_enabled() && !cl_points_.empty()) {
            draw_cl_lines(img);
        }
        else if (det_enabled() && detect_loader_) {
            zifImage *zimg = build_zifImage(img_rending_);
            const char *result = detect_loader_->detect(zimg);
            free_zifImage(zimg);

            DetectResult dr = parse_det_result(result);
            if (!dr.rcs.empty()) {
                draw_det_result(dr.rcs, img);
            }
        }

        draw_dr_history(img);   // 画可能的 ...

        painter->drawImage(r, *img);
        th_->unlock_picture(img_rending_);
        img_rending_ = 0;
    }
    else {
        if (info_.isEmpty()) {
            painter->drawText(0, 50, url_);
        }
        else {
            painter->drawText(10, 50, info_);
        }
    }
}

void MyPlayer::play()
{
    if (!th_) {
        first_audio_ = true;
        first_video_ = true;

        timer_.start();

        th_ = new MediaThread(url_.toStdString().c_str());

        ab_ = new AudioBuffer(th_);
        bool rc = ab_->open(QIODevice::ReadOnly);
        audio_output_->start(ab_);

        info_ = "loading ..." + url_;
        update();
    }
}

void MyPlayer::stop()
{
    if (th_) {
        audio_output_->stop();

        timer_.stop();

        if (img_rending_) {
            th_->unlock_picture(img_rending_);
            img_rending_ = 0;
        }

        delete th_;
        th_ = 0;

        info_ = "";
        update();
    }
}

AudioBuffer::AudioBuffer(MediaThread *mt)
    : mt_(mt)
{
    head_ = tail_ = 0;
    first_audio_ = true;
}

static qint64 silence(char *data, qint64 maxlen)
{
    int n = maxlen > 1024 ? 1024 : maxlen;
    memset(data, 0, n);
    return n;
}

qint64 AudioBuffer::readData(char *data, qint64 maxlen)
{
    // TODO: 这里根据音频数据的消耗，可以计算音频时间戳，并用于视频同步 ...

    load_from_mt(); // 总是尽量填充

    if (first_audio_) {
        // 多积累点儿声音数据，再开始投递 ...
        if (data_size() > 8*1024) {
            first_audio_ = false;
        }
        else {
            return silence(data, maxlen);
        }
    }

    if (maxlen <= 0) {
        return maxlen;
    }

    int bytes = maxlen <= data_size() ? maxlen : data_size();
    if (bytes < 512) {
        first_audio_ = true;
        qDebug("silence ...");

        // 返回静音数据 ...
        return silence(data, maxlen);
    }

    int de = CIRC_CNT_TO_END(head_, tail_, AU_BUFSIZE);
    if (de >= bytes) {
        memcpy(data, buf_+tail_, bytes);
    }
    else {
        memcpy(data, buf_+tail_, de);
        memcpy(data+de, buf_, bytes-de);
    }
    tail_ = (tail_+bytes) & (AU_BUFSIZE-1);

    return bytes;
}

qint64 AudioBuffer::writeData(const char *data, qint64 len)
{
    assert(0);
    return 0;
}

void MyPlayer::load_calibration_data()
{
    char *p = ::strdup(cfg_->get_value("calibration_data", ""));
    char *q = strtok(p, ";");
    while (q) {
        int x, y;
        if (sscanf(q, "%d,%d", &x, &y) == 2) {
            cl_points_.push_back(QPoint(x, y));
        }

        q = strtok(0, ";");
    }
    free(p);
}

void MyPlayer::when_cl_enabledChanged()
{
    if (cl_enabled()) {

    }
    else {

    }
}

int MyPlayer::cl_points()
{
    return cl_points_.size();
}

void MyPlayer::cl_push_point(int x, int y)
{
    if (cl_enabled()) {
        cl_points_.push_back(QPoint(x, y));
    }
}

void MyPlayer::cl_pop_point()
{
    if (cl_enabled() && !cl_points_.empty()) {
        cl_points_.pop_back();
    }
}

void MyPlayer::cl_remove_all_points()
{
    cl_points_.clear();
}

QString MyPlayer::cl_points_desc()
{
    std::stringstream ss;
    for (size_t i = 0; i < cl_points_.size(); i++) {
        int x = cl_points_[i].x() * atoi(cfg_->get_value("video_width", "480")) / width();
        int y = cl_points_[i].y() * atoi(cfg_->get_value("video_height", "270")) / height();
        ss << x << ',' << y << ';';
    }

    return ss.str().c_str();
}

void MyPlayer::cl_save()
{
    if (cl_enabled()) {
        QString s = cl_points_desc();
        cfg_->set_value("calibration_data", s.toStdString().c_str());
        cfg_->save_as();

        if (detect_loader_) {
            detect_loader_->reopen();
        }
    }
}

void MyPlayer::det_setEnabled(bool e)
{
    if (e) {
        if (!detect_loader_) {
            detect_loader_ = new DetectLoader(cfg_->get_value("detect_mod", "det_s.dll.dll"),
                                              cfg_->fname().toStdString().c_str());
        }
    }
    else {
        if (detect_loader_) {
            delete detect_loader_;
            detect_loader_ = 0;
        }
    }

    det_enabled_ = e;
}

void MyPlayer::det_set_params(double thres_dis, double thres_area, double factor_0, double factor_05)
{
    if (det_enabled_ && detect_loader_) {
        detect_loader_->set_param(thres_dis, thres_area, factor_0, factor_05, 0.0);
    }
}

void MyPlayer::dr_push(const QVariantMap &draw_desc)
{
    dr_opers.push(draw_desc);
}

void MyPlayer::dr_pop()
{
    if (!dr_opers.empty()) {
        dr_opers.pop();
    }
}

void MyPlayer::dr_clear()
{
    dr_opers.clear();
}
