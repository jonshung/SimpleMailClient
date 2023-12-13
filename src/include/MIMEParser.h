#ifndef _MIME_PARSER_H_
#define _MIME_PARSER_H_

#include <fstream>
#include <iostream>
#include <sstream>

#include "MIMEInterface.h"
#include "Encoder.h"

#include "mimetic/mimetic.h"

std::string fileToBase64(const std::string&, const int&);
MIMEAttachment mimeFromFile(const std::string&, const std::string & = "", const int& = 0);

mimetic::MimeEntity parseMIMEEntity(std::string);
std::string extractSubject(mimetic::MimeEntity*);
bool getMIMETextEntity(mimetic::MimeEntity*, mimetic::MimeEntity&);
void getMIMEAttachments(mimetic::MimeEntity*, std::vector<mimetic::MimeEntity*>&);

#endif