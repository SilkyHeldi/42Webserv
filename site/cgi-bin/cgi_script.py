#!/usr/bin/python3


import cgi
import os
import sys
import re
import io



def check_file_type(file_content):
    allowed_types = ['text/plain', 'application/pdf', 'image/png']
    
    file_type = magic.from_buffer(file_content, mime=True)
    
    if file_type in allowed_types:
        return 1
    else:
        return 0

def extract_text_from_post_data(post_data):
    start_delimiter = 'Content-Type: '
    end_delimiter = '\n\n'

    start_index = post_data.find(start_delimiter) + len(start_delimiter)
    end_index = post_data.find(end_delimiter, start_index)

    content_type = post_data[start_index:end_index].strip()

    start_delimiter = end_delimiter if content_type else '\n\n'
    end_delimiter = '\n---------------------------'

    start_index = post_data.find(start_delimiter) + len(start_delimiter)
    end_index = post_data.find(end_delimiter)

    extracted_text = post_data[start_index:end_index].strip()

    return extracted_text

def generate_response(status):
    response = 'HTTP/1.1 '
    response += status
    if (status == "404") :
        response += ' Not found\r\n'
    elif (status == "400") :
        response += ' Bad request\r\n'
    elif (status == "500") :
        response += ' Internal Server Error\r\n'
    elif (status == "201") :
        response += ' Created\r\n'
    response += 'Server: Webserv\r\n'

    if (status != "201") :
        with open(f"./site/errors/{status}.html", 'r') as file:
            file_contents = file.read()
    else :
        with open(f"./site/upload_succes.html", 'r') as file:
            file_contents = file.read()
    response += f'Content-Length: {len(file_contents)}\r\n'
    response += 'Content-Type: text/html; charset=utf-8\n'

    response += file_contents

    return response

def generate_download_response(file_path):
    # Check file existence
    file_path = "./site" + file_path
    #print(file_path)
    if os.path.isfile(file_path):
        # Open file in binary reading mode
        with open(file_path, 'rb') as file:
            # Read file content
            file_content = file.read()

        # Build HTTP response
        response = 'HTTP/1.1 200 OK\r\n'
        response += 'Content-Type: application/octet-stream\r\n'
        response += 'Content-Disposition: attachment; filename="{}"\r\n'.format(os.path.basename(file_path))
        response += 'Content-Length: {}\r\n'.format(len(file_content))
        response += '\n'

        response += file_content.decode()

        return response
    else:
        return generate_response("404")


RESET = "\033[0m"
BOLD = "\033[1m"
UNDERLINE = "\033[4m"
BLACK = "\033[30m"
RED = "\033[31m"
GREEN = "\033[32m"
YELLOW = "\033[33m"
BLUE = "\033[34m"
MAGENTA = "\033[35m"
CYAN = "\033[36m"
WHITE = "\033[37m"

def main():

    if os.environ.get('REQUEST_METHOD')  == 'POST':
        pipe_fd = sys.stdin.fileno()
        pipe_file = io.TextIOWrapper(os.fdopen(pipe_fd, 'rb'), encoding='utf-8')
        body = pipe_file.read()
        filename = "uploaded_file.txt"
        filename_match = re.search(r'filename="([^"]+)"', body)
        if filename_match:
            filename = filename_match.group(1)
        else:
            print("Filename not found.")
        upload_dir = "./site/uploads/"
        file_path = os.path.join(upload_dir, filename)

        # does the filename already exist ?
        if os.path.exists(file_path):
            # Split the filename and extension
            filename, extension = os.path.splitext(filename)

            # Generate the new filename with a version number
            version = 1
            new_filename = f"{filename}({version}){extension}"
            new_file_path = os.path.join(upload_dir, new_filename)

            # Increment the version number until a unique filename is found
            while os.path.exists(new_file_path):
                version += 1
                new_filename = f"{filename}({version}){extension}"
                new_file_path = os.path.join(upload_dir, new_filename)

            # Update the file path with the new filename
            file_path = new_file_path

        # Write the body content to the file
        with open(file_path, "w") as file:
            file.write(extract_text_from_post_data(body))

        # Check file existence
        if not os.path.exists(file_path):
            # Error message if file failed to be created
            sys.stdout.write(generate_response("500"))
            return
        sys.stdout.write(generate_response("201"))

    elif os.environ.get('REQUEST_METHOD')  == 'GET':
        sys.stdout.write(generate_download_response(os.environ.get('REQUEST_URI')))

if __name__ == "__main__":
    main()