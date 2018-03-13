/*!
 * \file EventReturnFilter.h
 * \author masc4ii
 * \copyright 2018
 * \brief This class filters return key from keyEvents
 */

#ifndef EVENTRETURNFILTER_H
#define EVENTRETURNFILTER_H

#include <QObject>
#include <QEvent>
#include <QKeyEvent>

class EventReturnFilter : public QObject
{
    Q_OBJECT
public:
    EventReturnFilter(QObject* parent = Q_NULLPTR);
protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // EVENTRETURNFILTER_H
