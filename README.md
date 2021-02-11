# discordbot
Multi-bot Multi-Threaded Asynchronous Discord Bot In C++

Run discordbot and for the first run it will not start any bots, click "Bot Setup" and enter your bot's name and api key and if you want tts enabled,
then click "Add Or Update" button, you can also configure more bots which will run from the same instance (not well tested though). So maybe just stick with one
for starting out testing it. After you restart after configuring at least one bot then it will begin working from there.

When compiling yourself don't forget to run windeployqt, macdeployqt, or linuxdeployqt, etc to build the dependencies and fill out of all the Qt stuff it needs.
(In the directory containing the compiled binary). If you don't it won't run, so just a heads up.

You should see it instantly pop into your discord(s) where it is added to, and instantly fire back responses, gotta go fast... :)
Current Commands:
1. $btc [optional amount] (or it just does 1.0BTC by default)
2. $stack [stack overflow query] (query stack overflow and respond with results)
3. $stacknext (view next results)
4. $stackconfig [num results per message] [num messages per response] (configure how to display results)
5. $git
/* Accept: $git [github_file_url] - Get file and post in up to 4 messages max or it uploads file as text
   $git[N] [github_file_url] - Get file and post in up to N messages max (does not upload file as text)
   $git [N] [github_file_url] - same as above
   (N / max posts clamped to 7) */
Current Auto Notifications:
1. When discord users stream over discord it notifies everyone in the last channel they were in that they are now streaming.

So it's pretty much plug and play :)
   
*Redesign Blueprint: (Main change is from asynchronous single-threaded into asynchronous multi-threaded)
*---Lightweight simple c++ https communication
    httpsclient [x]
    httpsrequest [x]
    httpsresponse [x]
*---Lightweight simple c++ discord api
    discordbot [x]
    discorduser [x]
    discordmessage [x]
*---Bot commander to create and automatically manage bot instances and their threads
    /w command function vectors and message/command string to mapped function executer (tried something different)
    command [x]
    commandarray [x]
    botcommander [x]
1.0 Release Features Checklist:
    -->Settings and data persistence [x]
    -->Github file fetch and post (in up to 7 messages (4 by default), or it uploads as a text file) [x]
    -->Stack overflow search [x]
    -->Bitcoin price checker [x]
    -->Implement btcgame
2.0 Release Redesign
*---Auto managing websocket service
    WebSocketService [x]
    -->Convert to multi-bot multi-threaded [x]
    -->Multiple auto managed websocket services [x]
