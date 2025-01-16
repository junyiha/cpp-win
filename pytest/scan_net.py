import threading
import nmap
from ping3 import ping

active_ip = []

def scan_network(ip_range, event):
    '''
        扫描网络
    '''
    for i in range(1, 255):
        ip = f"{ip_range}.{i}"
        print(f"test ip: {ip}\n")
        response = ping(ip, timeout=0.1)  # 设置超时
        if response:
            active_ip.append(ip)

    event.set()

def manual_scan():
    '''
        手动扫描
    '''
    threads = []
    events = []
    for i in range(1, 255):
        net = f"192.168.{i}"
        event = threading.Event()
        events.append(event)
        thread = threading.Thread(target=scan_network, args=(net,event,))
        threads.append(thread)
        thread.start()

    for e in events:
        e.wait()

    with open("C:/Users/anony/Documents/GitHub/cpp-win/data/active_ip.txt", "w") as file:
        for ip in active_ip:
            msg = str(f"active ip: {ip}\n")
            print(msg)
            file.write(msg)

def nmap_scan(ip):
    '''
        nmap工具扫描
    '''
    nm = nmap.PortScanner()

    nm.scan(ip, '22-8000')
    for host in nm.all_hosts():
        print(f"Host: {host} ({nm[host].hostname()})")
        print(f"State: {nm[host].state()}")
        for proto in nm[host].all_protocols():
            print(f"Protocol: {proto}")
            ports = nm[host][proto].keys()
            for port in ports:
                print(f"Port: {port}\tState: {nm[host][proto][port]['state']}")

def read_active_ip():
    '''
        获取在线IP
    '''
    data = []
    with open("C:/Users/anony/Documents/GitHub/cpp-win/data/active_ip.txt", "r") as file:
        data = file.readlines()

    for i in data:
        i = i.strip()
        i = i.split(":")[1]
        active_ip.append(i)

    for i in active_ip:
        print(i)

if __name__ == "__main__":

    for i in range(0, 255):
        ip = f"192.168.30.{i}"
        nmap_scan(ip)

    # read_active_ip()

    # for ip in active_ip:
    #     thread = threading.Thread(target=nmap_scan, args=(ip,))
    #     thread.start()