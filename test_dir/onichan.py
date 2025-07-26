#!/usr/bin/env python

import sys

# Ensure no extra output before headers
sys.stdout.write("Content-Type: text/html\r\n")
sys.stdout.write("\r\n")  # End of headers

# Now output valid HTML (ASCII-safe, no emojis)
sys.stdout.write("""\
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>CGI Test</title>
</head>
<body>
    <h1>Hello from CGI</h1>
    <p>This page was returned from a Python CGI script.</p>
</body>
</html>
""")
