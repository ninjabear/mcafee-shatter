McafeeShatter
=============

Implementation of a shatter attack (priv escalation) on a very old mcafee version.

The attack worked because Mcafee ran as a service with a window on the desktop. On one of the windows 
there was an "admin" button that allowed you to do some superuser operations, that was disabled.

This code hooks the mouse, waits for right clicks, and sends a left click event to the topmost child window under your right click.
If the code expects a button that is disabled not to recieve any click window messages, it will break. 
