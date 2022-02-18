#!/usr/bin/env python3

import cgi

form = cgi.FieldStorage()

message = "No file found :("

file_content = form["uploaded_file"]
if file_content.file:
 	with open(f"./uploaded_files/{file_content.filename}.uploaded", 'wb') as f:
			try:
				f.write(file_content.file.read())
			except Exception as e:
				message = f'"{file_content.filename}" couldn\'t be uploaded ({e}) :['
			else:
				message = f'"{file_content.filename}" uploaded with great success :)'


print("Content-type: text/html\r\n\r\n")
print("<html><body>")

print(f"<p>{message}</p>")

print("</body></html>")
