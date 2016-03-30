//
// Created by Leonardo on 17/03/2016.
//

#ifndef _MESSAGES_H
#define _MESSAGES_H

#define NO_MORE_SPACE   "Sorry, there is no more space in the chat room. See you later!\n"
#define KILL_CLIENT     "The server is no more available. Please try later!\n"
#define WELCOME         "Welcome to the chat room!\n"
#define SET_NAME        "Set a valid username (max 20 characters).\nUSERNAME: "
#define READY           "You are ready to chat. Puts '$help' to have a full command list you can use here, or puts $conn if you want to talk with one of the users below:\n"
#define HELPMESSAGE     "- $list: Shows the available users list\n- $conn: Require a connection with an available partner\nIf you want to disconnect from server press Ctrl+C\n"
#define WAITFORCMD      ">"
#define QUITMESS        "You are going to be disconnected. We hope to see you again!\n"
#define CHOOSE          "Insert the username you want to contact from the list below:\n"
#define SELECT          "PARTNER USERNAME: "
#define WAITCHAT        "Waiting for response...\n"
#define CHAT            "Chat session started! Puts '$exit' to disconnect from your partner\n"
#define TRY_LATER       "The user you are trying to contact is at the moment not available. Try later or choose another partner\n"
#define ASKING          " would like to start chatting with you! Would you like to do the same? y/n: "
#define REFUSE          " refuse your connection request.\n"
#define PLEASE          "Please answer 'y' to accept or 'n' to refuse: "
#define END_CHAT        " has gone. Press enter to continue\n"
#define NOBODY          "Nobody available at the moment.\n"
#define NOT_FOUND       "Partner not found.\n"

#endif
