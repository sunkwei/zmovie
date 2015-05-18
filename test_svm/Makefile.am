AUTOMAKE_OPTIONS= foreign

bin_PROGRAMS= train predict

train_SOURCES=	train.cpp common.cpp
train_LDADD= ${opencv_LIBS} ${libccgnu2_LIBS}

predict_SOURCES= predict.cpp common.cpp
predict_LDADD= ${opencv_LIBS} ${libccgnu2_LIBS}

