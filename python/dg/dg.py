import threading
import pystray
from pystray import Menu, MenuItem
from PIL import Image
from tkinter import Tk, Label, Button, Entry, Frame, PhotoImage, StringVar


def quit_window(icon: pystray.Icon):
    icon.stop()
    win.destroy()


def show_window():
    win.deiconify()


def hide_window():
    win.withdraw()


def on_click(icon, item):
    print("点击了", item)


def on_notify(icon: pystray.Icon):
    icon.notify("我是消息类容", "消息标题")


def on_pass():
    pass


def on_conn():
    pass


def on_login():
    hide_window()


if True:
    submenu1 = Menu(MenuItem('我的审批', action=on_click),
                    MenuItem('起草申请', action=on_click))
    submenu2 = Menu(MenuItem('授权离网', action=on_click),
                    MenuItem('时钟锁离网', action=on_click),
                    MenuItem('回到网络', action=on_click))
    
    menu = Menu(MenuItem('审批相关', submenu1),
                MenuItem('提醒设置', action=on_click),
                Menu.SEPARATOR,
                MenuItem('刷新策略', action=on_click),
                MenuItem('离网管理', submenu2),
                Menu.SEPARATOR,
                MenuItem('切换用户', action=quit_window),
                MenuItem('消息通知', action=show_window, default=True, visible=False))
    
    image = Image.open('D:/work/Prog/python/alg/dg/icon1.ico')
    icon = pystray.Icon('文档安全管理系统', image, '用户:qiuxuenan\n状态:在线', menu)

    win = Tk()
    win.geometry('339x230')
    win.protocol('WM_DELETE_WINDOW', hide_window)
    win.title('文档安全管理系统')
    win.iconbitmap('D:/work/Prog/python/alg/dg/icon1.ico')
    win.resizable = False
    win.config(bg='#f0f0f0')
    win.attributes('-toolwindow', 1)

    labeluser = Label(win, text='账号：').place(x=40, y=91, width=49, height=14)
    labelpass = Label(win, text='密码：').place(x=40, y=131, width=49, height=14)
    imagelogo = PhotoImage(file='D:/work/Prog/python/alg/dg/logo.png')
    labellogo = Label(win, image=imagelogo).place(x=3, y=3, width=333, height=65)
    framever = Frame(win, bg='#000000').place(x=0, y=204, width=339, height=1)
    labelver = Label(win, text='内核版本：5.0.2.41231', background='#f0eded').place(x=3, y=205, width=333, height=22)
    username = StringVar(win, value='qiuxuenan')
    password = StringVar(win, value='1234567890abcdef')
    entryuser = Entry(win, textvariable=username).place(x=94, y=87, width=217, height=25)
    entrypass = Entry(win, textvariable=password, show='*').place(x=94, y=127, width=217, height=25)
    btpass = Button(win, text='修改密码', command=on_pass).place(x=42, y=168, width=81, height=25)
    btconn = Button(win, text='连接设置', command=on_conn).place(x=136, y=168, width=81, height=25)
    btlogin = Button(win, text='登录', command=on_login).place(x=231, y=168, width=81, height=25)

    threading.Thread(target=icon.run, daemon=True).start()
    win.mainloop()
