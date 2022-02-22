import cgi

form = cgi.FieldStorage()

message = "file not found"

file = form["file"]
if file.file:
	with open(f"www/cgi/upload/{file.filename}", 'wb') as f:
		try:
			f.write(file.file.read())
		except Exception as e:
			message = 'file couldn\'t be uploaded'
		else:
			message = 'file successfully uploaded'

print("Content-type: text/html\r\n\r\n")
print(message)