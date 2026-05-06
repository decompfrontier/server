#pragma once

/*!
* StartDebugCli — called in normal server mode from registerBeginningAdvice.
* Creates a named pipe, spawns a second gimuserverw.exe instance in a new
* console window (the CLI window), and starts a detached pipe-server thread
* that dispatches commands against the live DB.
*
* RunDebugCliClient — called when the process is launched with
* "--debug-cli <pipename>". Connects to the named pipe and runs the
* interactive REPL on this process's own console (the separate window).
*
* Windows-only. On other platforms both functions are no-ops.
*/
void StartDebugCli();
void RunDebugCliClient(const char* pipeName);
