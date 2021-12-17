import sys
import numpy as np
from PyQt5 import QtGui, QtCore, QtWidgets
from PyQt5.QtCore import Qt
from PyQt5.QtGui import QPainter, QColor, QFont, QPen
from PyQt5.QtWidgets import QMainWindow, QApplication, QMessageBox


class TenWidget(QMainWindow):
    def __init__(self):
        super().__init__()
        self.w = 6
        self.h = 6
        self.s = 50
        self.g = 5
        self.p = None
        self.c = []
        self.init_data()
        self.init_ui()

    def init_ui(self):
        self.setFixedSize(self.w * (self.s + self.g) + self.g,
                          self.h * (self.s + self.g) + self.g)
        self.setWindowTitle('Ten')

    def init_data(self):
        self.data = np.random.randint(1, 5, [self.h, self.w])

    def calConnect(self, x, y):
        self.c.append((x, y))
        ps = [[x - 1, y], [x + 1, y], [x, y - 1], [x, y + 1]]
        for p in ps:
            px = p[0]
            py = p[1]
            if py < 0 or py >= self.h or px < 0 or px >= self.w:
                continue
            if self.data[py, px] != self.data[y, x]:
                continue
            if (px, py) in self.c:
                continue
            self.calConnect(px, py)

    def testStop(self):
        for y in range(self.h):
            for x in range(self.w):
                ps = [[x - 1, y], [x + 1, y], [x, y - 1], [x, y + 1]]
                for p in ps:
                    px = p[0]
                    py = p[1]
                    if py < 0 or py >= self.h or px < 0 or px >= self.w:
                        continue
                    if self.data[py, px] == self.data[y, x]:
                        return False
        return True

    def calNext(self):
        # 消除
        n = self.data[self.p[1], self.p[0]]
        for p in self.c:
            self.data[p[1], p[0]] = 0
        self.data[self.p[1], self.p[0]] = n + 1
        # 落下
        for x in range(self.w):
            for y in range(self.h - 1, -1, -1):
                if self.data[y, x] == 0:
                    for yy in range(y - 1, -1, -1):
                        if self.data[yy, x] != 0:
                            self.data[y, x] = self.data[yy, x]
                            self.data[yy, x] = 0
                            break
        # 生成
        for x in range(self.w):
            for y in range(self.h):
                if self.data[y, x] == 0:
                    self.data[y, x] = np.random.randint(1, 5)
                else:
                    break
        # 测试
        if self.testStop():
            b = QMessageBox.question(self, 'game over', 'begin a new game?',
                                     QMessageBox.Yes | QMessageBox.No, QMessageBox.Yes)
            if b == QMessageBox.Yes:
                self.init_data()
            else:
                QApplication.exit()

    def mousePressEvent(self, e: QtGui.QMouseEvent) -> None:
        if e.buttons() != QtCore.Qt.LeftButton:
            return
        p = e.pos()
        y = p.y() // (self.g + self.s)
        x = p.x() // (self.g + self.s)
        y1 = y * (self.g + self.s) + self.g
        x1 = x * (self.g + self.s) + self.g
        y2 = (y + 1) * (self.g + self.s)
        x2 = (x + 1) * (self.g + self.s)

        if p.x() < x1 or p.x() > x2 or p.y() < y1 or p.y() > y2:
            return

        self.p = (x, y)
        if len(self.c) > 1 and self.p in self.c:
            self.calNext()
            self.c = []
        else:
            self.c = []
            self.calConnect(x, y)
        self.update()

    def paintEvent(self, e) -> None:
        p = QPainter()
        p.begin(self)
        self.drawWidget(p)
        p.end()

    def drawWidget(self, p: QPainter):
        colors = [QColor(0xff, 0xff, 0xff),  # 0
                  QColor(0xff, 0x00, 0x00),  # 1
                  QColor(0x00, 0xff, 0x00),  # 2
                  QColor(0x00, 0x00, 0xff),  # 3
                  QColor(0xff, 0xff, 0x00),  # 4
                  QColor(0xff, 0x00, 0xff),  # 5
                  QColor(0x00, 0xff, 0xff),  # 6
                  QColor(0xff, 0x80, 0x00),  # 7
                  QColor(0x80, 0xff, 0x00),  # 8
                  QColor(0x00, 0xff, 0x80),  # 9
                  QColor(0x00, 0x80, 0xff),  # 10
                  QColor(0xff, 0x00, 0x80),  # 11
                  QColor(0x80, 0x00, 0xff),  # 12
                  QColor(0x80, 0x80, 0x00),  # 13
                  QColor(0x80, 0x80, 0x00),  # 14
                  QColor(0x00, 0x80, 0x80),  # 15
                  QColor(0x00, 0x80, 0x80),  # 16
                  QColor(0x80, 0x00, 0x80),  # 17
                  QColor(0x80, 0x00, 0x80),  # 18
                  ]
        for y in range(self.h):
            for x in range(self.w):
                n = self.data[y, x]
                r = [x * (self.g + self.s) + self.g, y * (self.g + self.s) + self.g, self.s, self.s]
                if (x, y) in self.c:
                    r = [x * (self.g + self.s), y * (self.g + self.s), self.s + self.g * 2, self.s + self.g * 2]
                if n < len(colors):
                    p.fillRect(r[0], r[1], r[2], r[3], colors[n])
                else:
                    p.fillRect(r[0], r[1], r[2], r[3],
                               QColor((n + 3) * 7 % 255, (n + 5) * 11 % 255, (n + 7) * 13 % 255))
                p.setFont(QFont("黑体", 30))
                p.setPen(Qt.black)
                p.drawText(r[0], r[1], r[2], r[3], Qt.AlignCenter, f'{n}')


def main():
    app = QApplication(sys.argv)
    win = TenWidget()
    win.show()
    app.exec_()


if __name__ == '__main__':
   main()
