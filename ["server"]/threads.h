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
#endif //TALKSERVER_THREADS_H
