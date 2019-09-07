#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import sys
import random
import numpy as np
from PyQt5 import QtGui, QtCore, QtWidgets
from PyQt5.QtGui import QPainter, QColor
from PyQt5.QtWidgets import QMainWindow, QApplication, QMessageBox

class Hanoi:
    def __init__(self, n):
        self.N = n
        self.H = [[i for i in range(n, 0, -1)], [], []]
        self.S = -1
        self.steps = 0
        self.init_map()
        self.update_map()

    def random(self):
        self.H = [[], [], []]
        for i in range(1, self.N + 1):
            j = random.randint(0, 2)
            self.H[j].append(i)
        self.steps = 0
        self.update_map()

    def win(self):
        for h in self.H:
            if len(h) != self.N:
                continue
            for j, p in enumerate(h):
                if j != self.N - p:
                    break
            else:
                return True
        return False

    def setSel(self, s):
        if s < 0 or s > 2 or len(self.H[s]) == 0:
            s = -1
        self.S = s
        self.update_map()

    def move(self, t):
        S = self.S
        T = t
        if S < 0 or S >= self.N or T < 0 or T >= self.N or S == T:
            return False

        if len(self.H[T]) == 0 or self.H[S][-1] < self.H[T][-1]:
            self.H[T].append(self.H[S].pop())
            self.S = -1
            self.steps += 1
            self.update_map()
            return True
        else:
            self.S = T
            self.update_map()
            return False

    def init_map(self):
        self.hborder = 2
        self.vborder = 1
        self.cols = 80
        self.rows = self.N + self.vborder * 2 + 2

        self.y0 = self.rows - 2 - self.vborder
        self.g = 4
        self.w = (self.cols - self.g * 4 - self.hborder * 2) // 3
        self.xc = [self.hborder + self.g + self.w // 2,
                   self.hborder + self.g * 2 + self.w + self.w // 2,
                   self.hborder + self.g * 3 + self.w * 2 + self.w // 2]

        self.M = np.zeros((self.rows, self.cols), dtype='int8')

    def update_map(self):
        self.M[:, :] = 0
        self.M[:self.vborder, :] = -1
        self.M[-self.vborder:, :] = -1
        self.M[:, :self.hborder] = -1
        self.M[:, -self.hborder:] = -1

        self.M[:, self.hborder + self.g + self.w + self.g // 2] = -1
        self.M[:, self.hborder + self.g * 2 + self.w * 2 + self.g // 2] = -1

        for i, h in enumerate(self.H):
            for j, p in enumerate(h):
                for k in range(-p, p - 1):
                    self.M[self.y0 - j, self.xc[i] + k] = p if self.S != i else -2

class HanoiWidget(QMainWindow):
    def __init__(self):
        super().__init__()
        self.hanoi = Hanoi(9)
        self.init_ui()
        self.game_new('begin', 'begin a new game ?')

    def init_ui(self):
        self.xrate = 10
        self.yrate = 30
        self.setFixedSize(self.hanoi.cols * self.xrate, self.hanoi.rows * self.yrate)
        self.setWindowTitle("hanoi")

    def game_new(self, s1, s2):
        button = QMessageBox.question(self, s1, s2,
                                      QMessageBox.Yes | QMessageBox.No | QMessageBox.Cancel,
                                      QMessageBox.Yes)
        if button == QMessageBox.Yes:
            self.hanoi.random()
        elif button == QMessageBox.No:
            pass
        else:
            QApplication.exit()

    def mousePressEvent(self, a0: QtGui.QMouseEvent) -> None:
        if a0.buttons() != QtCore.Qt.LeftButton:
            return

        p = a0.pos()
        i = (p.x() / self.xrate - self.hanoi.hborder - self.hanoi.g) / \
            (self.hanoi.w + self.hanoi.g / 2)
        i = max(0, min(int(i), 2))
        if self.hanoi.S == -1:
            self.hanoi.setSel(i)
            self.update()
        else:
            m = self.hanoi.move(i)
            self.update()
            if m and self.hanoi.win():
                self.game_new('win', 'you win, begin a new game ?')

    def paintEvent(self, e) -> None:
        painter = QPainter()
        painter.begin(self)
        self.drawWidget(painter)
        painter.end()

    def drawWidget(self, qp: QPainter):
        colors = [QColor(0xff, 0xff, 0xff),  # 0x00 bg
                  QColor(0xc0, 0xa0, 0xa0),  # 1
                  QColor(0xc0, 0x00, 0x60),  # 2
                  QColor(0xc0, 0x60, 0x00),  # 3
                  QColor(0xa0, 0xc0, 0x20),  # 4
                  QColor(0x00, 0xc0, 0x80),  # 5
                  QColor(0x40, 0x80, 0x80),  # 6
                  QColor(0x80, 0x40, 0xc0),  # 7
                  QColor(0x40, 0x40, 0xc0),  # 8
                  QColor(0x40, 0xa0, 0xc0),  # 9
                  QColor(0x40, 0x40, 0x40),  # -2 sel
                  QColor(0x00, 0x00, 0x00), ]  # -1 border
        for i in range(0, self.hanoi.rows):
            for j in range(0, self.hanoi.cols):
                h = self.hanoi.M[i, j]
                qp.fillRect(j * self.xrate, i * self.yrate, self.xrate, self.yrate, colors[h])

def draw_console(h: Hanoi):
    modeReset = '\033[0m'
    modes = ['\033[0;30;30m',  # 0 bg      black
             '\033[0;30;47m',  # 1         white
             '\033[0;30;47m',  # 2         white
             '\033[0;30;47m',  # 3         white
             '\033[0;30;47m',  # 4         white
             '\033[0;30;47m',  # 5         white
             '\033[0;30;47m',  # 6         white
             '\033[0;30;47m',  # 7         white
             '\033[0;30;47m',  # 8         white
             '\033[0;30;47m',  # 9         white
             '\033[0;34;44m',  # -2 sel    blue
             '\033[0;33;43m', ]  # -1 border yellow
    os.system('clear')
    for i in range(0, h.rows):
        for j in range(0, h.cols):
            h = h.M[i, j]
            sys.stdout.write("{}{}{}".format(modes[h],
                h if 0 < h <= h.N else ' ', modeReset))
        print("")
    print(h.H, ' steps: ', h.steps)

def play_console(h: Hanoi):
    while True:
        while h.S == -1:
            f = input("move from: ")
            h.setSel(int(f) - 1)
            draw_console(h)

        t = input("move to  : ")
        m = h.move(int(t) - 1)
        draw_console(h)
        if m and h.win():
            break

def begin_console(h: Hanoi):
    draw_console(h)
    while True:
        s = input("Type 'p' to play  \n     'r' to random \n     'q' to quit \n")
        if s == 'q':
            break
        elif s == 'r':
            h.random()
            draw_console(h)
        elif s == 'p':
            play_console(h)

def start_gui():
    app = QApplication(sys.argv)
    widget = HanoiWidget()
    widget.show()
    sys.exit(app.exec_())

def start_cmd():
    n = input("Input count (max 9): ")
    N = max(3, min(9, int(n)))
    h = Hanoi(N)
    begin_console(h)

def usage():
    print(sys.argv[0], 'Option')
    print('    -g graphics version')
    print('    -c console version')

if __name__ == "__main__":
    a = sys.argv
    if len(a) != 2:
        usage()
        exit()

    if a[1] == '-g':
        start_gui()
    elif a[1] == '-c':
        start_cmd()
    else:
        usage()
