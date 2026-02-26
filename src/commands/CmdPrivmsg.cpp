
// IRC PRIVMSG command
// Syntax: PRIVMSG <target> :<message>
// Target can be a #channel or a nickname


#include "../../includes/CmdPrivmsg.hpp"
#include "../../includes/ClientUser.hpp"

void CmdPrivmsg::execute(Server& server, ClientUser& clientUser, const ParsedCommand& cmd)
{
    // Must be registered
    if (!clientUser.isRegistered())
    {
        clientUser.get_outputBuffer().append(
            ":server 451 * :You have not registered\r\n");
        return;
    }

    // PRIVMSG <target> :<text>  → params[0] = target, params[1] = text
    if (cmd.params.size() < 2)
    {
        clientUser.get_outputBuffer().append(
            ":server 461 " + clientUser.getNickname() + " PRIVMSG :Not enough parameters\r\n");
        return;
    }

    const std::string& target  = cmd.params[0];
    const std::string& text    = cmd.params[1];

    if (text.empty())
    {
        clientUser.get_outputBuffer().append(
            ":server 412 " + clientUser.getNickname() + " :No text to send\r\n");
        return;
    }

    std::string prefix = ":" + clientUser.getNickname() + "!" +
                         clientUser.getUsername() + "@ircserver";
    std::string msgOut = prefix + " PRIVMSG " + target + " :" + text + "\r\n";

    // ── Channel message ──────────────────────────────────────────────────────
    if (target[0] == '#')
    {
        if (!server.channelExists(target))
        {
            clientUser.get_outputBuffer().append(
                ":server 403 " + clientUser.getNickname() + " " + target +
                " :No such channel\r\n");
            return;
        }

        Channel& channel = server.getChannel(target);
        if (!channel.hasMember(clientUser.get_ClientUser_fd()))
        {
            // ERR_CANNOTSENDTOCHAN
            clientUser.get_outputBuffer().append(
                ":server 404 " + clientUser.getNickname() + " " + target +
                " :Cannot send to channel\r\n");
            return;
        }

        // Broadcast to every member EXCEPT the sender
        server.broadcastToChannelExcept(target, msgOut, clientUser.get_ClientUser_fd());
        return;
    }

    // ── Private message to a nick ────────────────────────────────────────────
    ClientUser* recipient = server.getClientByNick(target);
    if (!recipient)
    {
        // ERR_NOSUCHNICK
        clientUser.get_outputBuffer().append(
            ":server 401 " + clientUser.getNickname() + " " + target +
            " :No such nick\r\n");
        return;
    }

    recipient->get_outputBuffer().append(msgOut);
}
