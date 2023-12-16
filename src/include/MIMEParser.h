#ifndef _MIME_PARSER_H_
#define _MIME_PARSER_H_

#include <fstream>
#include <iostream>
#include <sstream>

#include "MIMEInterface.h"
#include "Encoder.h"
#include "Parser.h"

std::string fileToBase64(const std::string&, const int&);
MIMEAttachment mimeFromFile(const std::string&, const std::string & = "", const int& = 0);

MIMESegment parseMIMEEntity(std::string);
std::string extractSubject(MIMESegment*);
bool getMIMETextEntity(MIMESegment*, MIMESegment&);
void getMIMEAttachments(MIMESegment, std::vector<MIMESegment>&);


#endif