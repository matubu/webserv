#!/usr/local/bin/python3

import argparse
import requests
import socket
import time

def client_connect(url):
	# http.client.HTTPConnection(addr, port, timeout=1000)
	r = requests.get(url)
	print(r.status_code)

def socket_connect(host, port):
	with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
		s.connect((host, port))
		s.send(b'Hi !\r\n')
		data = s.recv(1)

def spam_connect(addr, port, client_amount):
	for _ in range(args.client_amount):
		client_connect(f"{args.addr}:{args.port}/")

def stall_connect(host, port):
	with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
		s.connect((host, port))

		socket_connect(host, port)
		
		s.send(b'Hello, world\r\n')
		data = s.recv(1)
	print("stopped stalling")

parser = argparse.ArgumentParser()
parser.add_argument("addr", type=str, default="localhost")
parser.add_argument("port", type=int, default=80)
parser.add_argument("-client_amount", type=int, default=1)
args = parser.parse_args()

# spam_connect(args.addr, args.port, args.client_amount)
stall_connect(args.addr, args.port)
