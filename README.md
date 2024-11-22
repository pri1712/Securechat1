# Secure CHAT
Allows users on the same LAN to communicate together , but allows only 2 users per chat room and users cant be in multiple chat rooms.
Uses multithreading.

Features:

Multi-client Support: Handles multiple clients simultaneously using threads. But a client cant be in more than one chat-room at once , this functionality can be extended later.

Account Management: Supports account creation, login, and logout functionalities.

Message Encryption: Encrypts messages using RC4 and a key that is derived using the DH key exchange algo
Concurrency Control: Ensures thread-safe access to shared resources using mutexes.
