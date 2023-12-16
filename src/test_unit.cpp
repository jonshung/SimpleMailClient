#include "test_unit.h"
#include "Parser.h"
#include "Encoder.h"

MailContent getMailContent(MailboxAddress sender) {
    MailContent _mailContent;
    std::string buffer;
    std::cout << "Mailing system\n\n";
    _mailContent.setFrom(sender.getReceiptName(), sender.getReceiptAddress());

    std::cout << "Please enter reciever address: ";
    std::getline(std::cin, buffer);
    _mailContent.addTo("", buffer);

    std::cout << "Please enter the number of cc: ";
    int ccCount = 0;
    std::cin >> ccCount;
    std::cin.ignore(1, '\n');

    for (int i = 0; i < ccCount; i++) {
        std::cout << "Please enter the " << (i + 1) << " cc recipent: ";
        std::getline(std::cin, buffer);
        _mailContent.addCC("", buffer);
    }

    std::cout << "Please enter the subject: ";
    std::getline(std::cin, buffer);
    _mailContent.setSubject("");

    std::cout << "Please enter content, ending with a line containing only \"<END>\": " << std::endl;
    std::stringstream plaintext;
    std::stringstream htmltext;
    while (1) {
        std::getline(std::cin, buffer);
        if (buffer == "<END>") break;
        plaintext << buffer << "\r\n";
        htmltext << buffer << "<br>";
    }

    //_mailContent.setPlainContent(plaintext.str());

    std::string htmlString =
        "<html>\r\n"
        "   <body>\r\n"
        "       <p>\r\n"
        "            " + htmltext.str() + "\r\n"
        "       </p>\r\n"
        "   </body>\r\n"
        "</html>";
    _mailContent.setHTMLContent(htmlString);
    std::cout << "Please enter the number of attachment: ";
    int attachNum;
    std::cin >> attachNum;
    std::cin.ignore(1, '\n');
    for (int i = 0; i < attachNum; i++) {
        std::string filePath;
        std::cout << "Please enter a filepath: ";
        std::getline(std::cin, filePath);
        std::cout << "Please enter a description, ending with <END>: " << std::endl;
        std::stringstream descBuffer;
        while (1) {
            std::getline(std::cin, buffer);
            if (buffer == "<END>") break;
            descBuffer << buffer;
        }

        MIMEAttachment _mime = mimeFromFile(filePath, descBuffer.str(), 20 * 1024 * 1024); // 20MB max
        if (_mime.getData().length() == 0) {
            std::cout << "File ignored" << std::endl;
            continue;
        }
        _mailContent.addAttachment(_mime);
    }
    return _mailContent;
}

/*void test2(std::string _port) {
    std::string _profile;
    std::cout << "Enter a profile's mailbox address: ";
    std::getline(std::cin, _profile);

    POP3Client tester("localhost", _port, _profile, _profile);
    std::tuple<int, int> statCmd = tester.stat();
    std::cout << "Total messages: " << std::get<0>(statCmd) << ", octet(s): " << std::get<1>(statCmd) << std::endl;
    std::cout << "listing: " << std::endl;
    std::vector< std::tuple<int, int> > listing = tester.list();
    for (std::tuple<int, int> item : listing) {
        std::cout << "[+] " << std::get<0>(item) << ": " << std::get<1>(item) << std::endl;
    }
    std::cout << "Select a message to download, or \"<quit>\" to quit: ";
    std::string buffer;
    std::getline(std::cin, buffer);
    if (buffer == "<quit>") {
        std::cout << "Quitting....\n";
        return;

    }
    std::vector<std::string> msg;
    if (!tester.get(msg, buffer)) {
        std::cout << "error: " << tester._error << std::endl;
    }
    if (msg.size() > 0) {
        std::cout << std::endl << "Got message " << buffer << "(" << msg.size() << " lines)!" << std::endl;
        std::fstream helper("output.eml", std::fstream::out | std::fstream::binary);
        helper << POP3Client::serializeString(msg);
        helper.close();
        std::cout << std::endl << "End transmission!" << std::endl << std::endl;
    }
}*/

void printSeg(MIMESegment e) {
    //qDebug() << e.header().get("Content-Type").value();
    for (MIMESegment seg : e.body().parts()) {
        printSeg(seg);
        if(seg.header().get("Content-Type").value() == "text/html") {
            qDebug().noquote() << Encoder::decode_quoted_printable(seg.body().rawContent());
        }
        if (seg.header().get("Content-Disposition").value().find("attachment") != std::string::npos) {
            std::ofstream fileHandler("profile_picture2.jpg");
            if(!fileHandler.is_open()) continue;
            fileHandler << Encoder::decode_base64(seg.body().rawContent(), true);
            fileHandler.close();
        }
    }
}

void evalTest() {
    /*std::string _user, _addr, _password;

    std::cout << "user: ";
    std::getline(std::cin, _user);
    std::cout << "address: ";
    std::getline(std::cin, _addr);
    std::cout << "password: ";
    std::getline(std::cin, _password);

    MailboxAddress sender(_user, _addr);
    MailContent mCt = getMailContent(sender);

    ClientMailbox client(Credential(sender, _password));
    std::cout << "Send test\r\n\r\n";
    client.sendContent("localhost", "2225", mCt);
    std::cout << "Sent!\r\n";
    std::cout << "Fetching back\r\n";
    client.fetch("localhost", "2226");
    std::cout << "Fetched!\r\n";*/

    std::ifstream fileHandler("mailboxes/jonsphilogy@gmail.com/Inbox/20231215025830273.msg");
    if(!fileHandler.is_open()) {
        std::cout << "Cannot open file\r\n";
        return;
    }
    std::string content((std::istreambuf_iterator<char>(fileHandler)), std::istreambuf_iterator<char>());
    //parseMIMEEntity(content);
    MIMESegment e(content);
    printSeg(e);

    fileHandler.close();
}

