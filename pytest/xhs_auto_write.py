'''
    小红书自动发文章
'''
import webbrowser

url = 'https://www.xiaohongshu.com/explore'

# 指定 Microsoft Edge 的路径（Windows 系统默认路径）
edge_path = r"C:\Program Files (x86)\Microsoft\Edge\Application\msedge.exe"

# 注册 Microsoft Edge
webbrowser.register('edge', None, webbrowser.BackgroundBrowser(edge_path))

# 使用 Edge 打开指定网址
webbrowser.get('edge').open(url)
