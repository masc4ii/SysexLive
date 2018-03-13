/*!
 * \file EventReturnFilter.cpp
 * \author masc4ii
 * \copyright 2018
 * \brief This class filters return key from keyEvents
 */

#include "EventReturnFilter.h"

EventReturnFilter::EventReturnFilter( QObject* parent )
: QObject( parent ) {

}

bool EventReturnFilter::eventFilter( QObject *obj, QEvent *event )
{
    if( event->type() == QEvent::KeyPress )
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>( event );
        //qDebug( "Ate key press %d", keyEvent->key() );
        if( keyEvent->key() == Qt::Key_Return )
        {
            return true;
        }
        else
        {
            return QObject::eventFilter( obj, event );
        }
    }
    else
    {
        // standard event processing
        return QObject::eventFilter( obj, event );
    }
}
