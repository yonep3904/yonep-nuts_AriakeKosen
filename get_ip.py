import socket

def main():
    ip = get_ip()
    print(ip)

def get_ip():
    try:
        hostname = socket.gethostname()
        return socket.gethostbyname(hostname)
    except Exception as e:
        return f'Error: {e}'
    
if __name__ == '__main__':
    main()