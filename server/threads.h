//
// Created by Leonardo on 17/03/2016.
//

#ifndef TALKSERVER_THREADS_H
#define TALKSERVER_THREADS_H
//connection handler thread
void* _connection_handler(void* args);

/*
 * Manage chat signal
 */
void _chat_signal();

/*
 * Manage interrupt signal
 */
void killClient();
void killServer();
void alarm_man();
#endif //TALKSERVER_THREADS_H
