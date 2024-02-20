# -*- coding: utf-8 -*-
"""
@author     : mjsaka
@contact    : qiuxuenan@gmail.com
@time       : 2024/2/6 10:20
@file       : ui.py
@description: ui
"""

import sys
import json
from lib.conf import Conf

from lib.excel import ExcelChecker

from PySide6.QtCore import (Qt, Slot, Signal, QObject, QTimer, QSize)
from PySide6.QtGui import (QStandardItemModel, QStandardItem, QColor, QTextCursor, QIcon, QAction)
from PySide6.QtWidgets import (QApplication, QMainWindow, QWidget, QFileDialog, QMessageBox,
                               QPushButton, QTextEdit, QLabel,
                               QAbstractItemView, QTreeView, QProgressBar, QToolBar,
                               QVBoxLayout, QHBoxLayout, QStyle, QStyleFactory)
import qdarkstyle


class SysConf(Conf):
    def __init__(self):
        super().__init__()
        self.show_error_only = False
        self.ignore_space = False
        self.ignore_case = False
        self.ignore_symbol = False
        self.expand_depth = 1

    def save(self):
        self.dump('sys_conf.json')

    def read(self):
        self.load('sys_conf.json')


sys_conf = SysConf()
sys_conf.read()


class TextEditIO(QObject):
    """
    标准输出流重定向至QText中
    """
    output_signal = Signal(str)

    def __init__(self):
        super().__init__()
        self.buf = ''

    def write(self, s):
        self.buf += s
        idx = self.buf.find('\n')
        if idx >= 0:
            self.output_signal.emit(self.buf[:idx])
            self.buf = self.buf[idx+1:]

    def flush(self):
        pass


class TreeWindow(QWidget):
    def __init__(self, parent):
        super().__init__(parent)

        self.view = QTreeView(self)
        self.view.setAlternatingRowColors(False)
        self.view.setEditTriggers(QAbstractItemView.NoEditTriggers)
        self.view.setSelectionBehavior(QAbstractItemView.SelectRows)
        self.view.setHorizontalScrollMode(QAbstractItemView.ScrollPerPixel)
        self.view.setAnimated(False)
        self.view.setAllColumnsShowFocus(True)

        font = self.view.font()
        font.setPointSize(14)
        font.setBold(True)
        self.view.setFont(font)

        # 设置树视图的数据模型为之前创建的标准项模型
        self.item = QStandardItemModel(self)
        self.init_tree_item(None)

        self.view.setModel(self.item)

        # 布局设置
        layout = QHBoxLayout()
        layout.addWidget(self.view, stretch=1)
        layout.setContentsMargins(0,0,0,0)

        self.setLayout(layout)

    def update_tree(self):
        # 调整列宽
        self.view.setColumnWidth(0, 200)
        self.view.setColumnWidth(1, 300)
        self.view.setColumnWidth(2, 300)
        # 展开树视图
        if sys_conf.expand_depth < 0:
            self.view.collapseAll()
        elif sys_conf.expand_depth > 0:
            self.view.expandAll()
        else:
            self.view.expandToDepth(sys_conf.expand_depth)

    def is_equal(self, str1: str, str2: str):
        """自定义比较字符串是否相等
        :param str1:
        :param str2:
        :return: 相等返回True
        """
        # 忽略空白字符
        if sys_conf.ignore_space:
            str1 = str1.replace(' ', '').replace('\t', '')
            str2 = str2.replace(' ', '').replace('\t', '')
        # 忽略大小写
        if sys_conf.ignore_case:
            str1 = str1.upper()
            str2 = str2.upper()

        if len(str1) != len(str2):
            return False

        punc_half = """()-[]''"";:,.<>"""
        punc_full = """（）－【】‘’“”；：，。《》"""

        for i in range(len(str1)):
            if str1[i] == str2[i]:
                continue

            # 检查是否符号半角全角不同
            if sys_conf.ignore_symbol:
                i1 = punc_full.find(str1[i])
                i2 = punc_full.find(str2[i])

                if i1 >= 0 and str2[i] == punc_half[i1]:
                    continue

                if i2 >= 0 and str1[i] == punc_half[i2]:
                    continue

            return False

        return True

    def check_row(self, row_data):
        # 检查某一行数据是否匹配
        row_name = row_data['name']
        row_model = row_data['model']

        if row_data['records']:
            for record_data in row_data['records']:
                name = record_data['name']
                model = record_data['model']

                if not self.is_equal(row_name, name):
                    return False
                if not self.is_equal(row_model, model):
                    return False

        return True

    def init_record_item(self, row_item, row_data, record_data):
        # 添加一条数据库记录至树视图的行记录中
        row_name = row_data['name']
        row_model = row_data['model']
        name = record_data['name']
        model = record_data['model']
        record_item = QStandardItem('')
        name_item = QStandardItem('    ' + name)
        model_item = QStandardItem('    ' + model)

        # 不匹配的字段显示为红色
        if self.is_equal(row_name, name):
            name_item.setForeground(QColor('green'))
        else:
            name_item.setForeground(QColor('red'))

        if self.is_equal(row_model, model):
            model_item.setForeground(QColor('green'))
        else:
            model_item.setForeground(QColor('red'))

        # 添加到行中
        row_item.appendRow(record_item)
        row_item.setChild(record_item.index().row(), 1, name_item)
        row_item.setChild(record_item.index().row(), 2, model_item)

    def init_row_item(self, sheet_item, row, row_data):
        # 添加Excel一行数据至树视图中
        if sys_conf.show_error_only:
            if row_data['type'] != 'section' and self.check_row(row_data):
                return

        name = row_data['name']
        model = row_data['model']
        row_item = QStandardItem(row)
        name_item = QStandardItem(name)
        model_item = QStandardItem(model)

        # 颜色处理
        if row_data['type'] == 'section':
            row_item.setBackground(QColor('blue'))
            name_item.setBackground(QColor('blue'))
            model_item.setBackground(QColor('blue'))
        elif not self.check_row(row_data):
            row_item.setBackground(QColor('magenta'))
            name_item.setBackground(QColor('magenta'))
            model_item.setBackground(QColor('magenta'))

        # 执行Excel行数据添加
        sheet_item.appendRow(row_item)
        sheet_item.setChild(row_item.index().row(), 1, name_item)
        sheet_item.setChild(row_item.index().row(), 2, model_item)

        # 数据库记录添加
        if row_data['records']:
            for record_data in row_data['records']:
                self.init_record_item(row_item, row_data, record_data)

    def init_tree_item(self, data):
        # 清空数据
        self.item.clear()
        self.item.setHorizontalHeaderLabels(['行号', '名称', '图号'])

        if not data:
            return

        for sheet, sheet_data in data.items():
            # 表名数据添加
            sheet_item = QStandardItem(sheet)
            self.item.appendRow(sheet_item)

            # 每行数据添加
            for row, row_data in sheet_data.items():
                self.init_row_item(sheet_item, row, row_data)

        self.update_tree()


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        self.setWindowTitle("BOM图号校对")
        self.setGeometry(100, 100, 900, 600)
        self.setAcceptDrops(True)

        # 重定向标准输出
        self.stdout = TextEditIO()
        self.stdout.output_signal.connect(self.update_stdout)
        sys.stdout = self.stdout
        sys.stderr = self.stdout

        self.checker = None
        self.data = {}
        # 创建树形控件
        self.tree = TreeWindow(self)
        self.log = QTextEdit(self)

        self.init_progressbar()
        self.init_menu()
        self.init_toolbar()
        self.init_layout()

    def init_progressbar(self):
        self.progressinfo = QLabel()
        self.progressbar = QProgressBar(self)

    def init_layout(self):
        # 布局设置
        layout_progress = QHBoxLayout()
        layout_progress.addWidget(self.progressbar, stretch=1)
        layout_progress.addWidget(self.progressinfo, stretch=1)

        layout = QVBoxLayout()
        layout.addWidget(self.tree, stretch=1)
        layout.addWidget(self.log, stretch=0)
        layout.addLayout(layout_progress, stretch=0)
        layout.setContentsMargins(3,3,3,3)

        container = QWidget()
        container.setLayout(layout)
        self.setCentralWidget(container)


    def init_toolbar(self):
        self.toolbar = QToolBar(self)
        self.toolbar.setIconSize(QSize(32, 32))
        self.toolbar.setToolButtonStyle(Qt.ToolButtonStyle.ToolButtonIconOnly)
        self.toolbar.setMovable(False)
        self.toolbar.setFloatable(False)
        self.toolbar.setAllowedAreas(Qt.ToolBarArea.LeftToolBarArea)

        self.addToolBar(self.toolbar)

        self.toolbar.addAction(self.open_action)
        self.toolbar.addAction(self.exit_action)

        self.toolbar.addSeparator()

        self.toolbar.addAction(self.collapse_action)
        self.toolbar.addAction(self.expand_action)

        self.toolbar.addSeparator()

        self.show_error_toolbar_action = QAction("只显示异常")
        self.show_error_toolbar_action.setIcon(QIcon('icons/filter.png'))
        self.show_error_toolbar_action.triggered.connect(self.show_error)
        self.show_error_toolbar_action.setCheckable(True)
        self.show_error_toolbar_action.setChecked(sys_conf.show_error_only)

        self.ignore_space_toolbar_action = QAction("忽略空白")
        self.ignore_space_toolbar_action.setIcon(QIcon('icons/space.png'))
        self.ignore_space_toolbar_action.triggered.connect(self.ignore_space)
        self.ignore_space_toolbar_action.setCheckable(True)
        self.ignore_space_toolbar_action.setChecked(sys_conf.ignore_space)

        self.ignore_case_toolbar_action = QAction("忽略大小写")
        self.ignore_case_toolbar_action.setIcon(QIcon('icons/case.png'))
        self.ignore_case_toolbar_action.triggered.connect(self.ignore_case)
        self.ignore_case_toolbar_action.setCheckable(True)
        self.ignore_case_toolbar_action.setChecked(sys_conf.ignore_case)

        self.ignore_symbol_toolbar_action = QAction("忽略标点")
        self.ignore_symbol_toolbar_action.setIcon(QIcon('icons/symbol.png'))
        self.ignore_symbol_toolbar_action.triggered.connect(self.ignore_symbol)
        self.ignore_symbol_toolbar_action.setCheckable(True)
        self.ignore_symbol_toolbar_action.setChecked(sys_conf.ignore_symbol)

        self.toolbar.addAction(self.show_error_toolbar_action)
        self.toolbar.addAction(self.ignore_space_toolbar_action)
        self.toolbar.addAction(self.ignore_case_toolbar_action)
        self.toolbar.addAction(self.ignore_symbol_toolbar_action)


    def init_menu(self):
        self.menubar = self.menuBar()

        self.file_menu = self.menubar.addMenu("文件")

        self.open_action = self.file_menu.addAction("打开Excel")
        self.open_action.setIcon(QIcon('icons/excel.png'))
        self.open_action.setShortcut("Ctrl+N")
        self.open_action.triggered.connect(self.open_excel)

        self.open_action2 = self.file_menu.addAction("打开Json")
        self.open_action2.setIcon(QIcon('icons/json.png'))
        self.open_action2.setShortcut("Ctrl+O")
        self.open_action2.triggered.connect(self.open_json)

        self.file_menu.addSeparator()

        self.exit_action = self.file_menu.addAction("退出")
        self.exit_action.setIcon(QIcon('icons/close.png'))
        self.exit_action.setShortcut("Ctrl+Q")
        self.exit_action.triggered.connect(self.close)

        self.view_menu = self.menubar.addMenu("视图")

        self.collapse_action = self.view_menu.addAction("折叠")
        self.collapse_action.setIcon(QIcon('icons/collapse.png'))
        self.collapse_action.setShortcut("Ctrl+C")
        self.collapse_action.triggered.connect(self.collapse_tree)

        self.expand_action = self.view_menu.addAction("展开")
        self.expand_action.setIcon(QIcon('icons/expand.png'))
        self.expand_action.setShortcut("Ctrl+E")
        self.expand_action.triggered.connect(self.expand_tree)

        self.view_menu.addSeparator()

        self.show_error_action = self.view_menu.addAction("只显示异常")
        self.show_error_action.triggered.connect(self.show_error)
        self.show_error_action.setCheckable(True)
        self.show_error_action.setChecked(sys_conf.show_error_only)

        self.ignore_space_action = self.view_menu.addAction("忽略空白")
        self.ignore_space_action.triggered.connect(self.ignore_space)
        self.ignore_space_action.setCheckable(True)
        self.ignore_space_action.setChecked(sys_conf.ignore_space)

        self.ignore_case_action = self.view_menu.addAction("忽略大小写")
        self.ignore_case_action.triggered.connect(self.ignore_case)
        self.ignore_case_action.setCheckable(True)
        self.ignore_case_action.setChecked(sys_conf.ignore_case)

        self.ignore_symbol_action = self.view_menu.addAction("忽略标点")
        self.ignore_symbol_action.triggered.connect(self.ignore_symbol)
        self.ignore_symbol_action.setCheckable(True)
        self.ignore_symbol_action.setChecked(sys_conf.ignore_symbol)

    @Slot(str)
    def update_stdout(self, output):
        self.log.append(output)
        self.log.moveCursor(QTextCursor.MoveOperation.End)

    @Slot(dict)
    def update_progress(self, info):
        self.progressinfo.setText(info['info'])

        self.progressbar.setMaximum(info['total'])
        self.progressbar.setValue(info['current'])

    @Slot(dict)
    def finish_open(self, data):
        self.checker = None
        self.progressinfo.setText('处理完成，请检查核对结果')

        self.progressbar.setMaximum(100)
        self.progressbar.setValue(100)

        self.data = data
        self.tree.init_tree_item(data)
        self.setWindowTitle("BOM图号校对  " + self.file_name)

    def dragEnterEvent(self, event):
        file = event.mimeData().urls()[0].toLocalFile()
        print('dragEnterEvent', file)
        if file.endswith('.xls') or file.endswith('.xlsx'):
            event.setDropAction(Qt.DropAction.TargetMoveAction)
            event.accept()

    def dropEvent(self, event):
        file = event.mimeData().urls()[0].toLocalFile()
        if file.endswith('.xls') or file.endswith('.xlsx'):
            self.file_name = file
            self.check_excel()

    def check_excel(self):
        if self.checker is None:
            self.checker = ExcelChecker(self.file_name)
            self.checker.progress_signal.connect(self.update_progress)
            self.checker.finish_signal.connect(self.finish_open)
            self.checker.start()

    def open_excel(self):
        self.file_name, _ = QFileDialog.getOpenFileName(self, '打开文件', '', 'Excel表格文件 (*.xls *.xlsx)')
        if self.file_name:
            self.check_excel()

    def open_json(self):
        self.file_name, _ = QFileDialog.getOpenFileName(self, '打开文件', '', '数据文件 (*.json)')
        if self.file_name:
            with open(self.file_name, 'r', encoding='utf8') as f:
                self.data = json.load(f)
                self.tree.init_tree_item(self.data)
                self.setWindowTitle("BOM图号校对  " + self.file_name)

    def collapse_tree(self):
        sys_conf.expand_depth -= 1
        if sys_conf.expand_depth < -1:
            sys_conf.expand_depth = -1
        sys_conf.save()
        self.tree.update_tree()

    def expand_tree(self):
        sys_conf.expand_depth += 1
        if sys_conf.expand_depth > 1:
            sys_conf.expand_depth = 1
        sys_conf.save()
        self.tree.update_tree()

    def show_error(self):
        sys_conf.show_error_only = not sys_conf.show_error_only
        sys_conf.save()
        self.show_error_action.setChecked(sys_conf.show_error_only)
        self.show_error_toolbar_action.setChecked(sys_conf.show_error_only)
        self.tree.init_tree_item(self.data)

    def ignore_space(self):
        sys_conf.ignore_space = not sys_conf.ignore_space
        sys_conf.save()
        self.ignore_space_action.setChecked(sys_conf.ignore_space)
        self.ignore_space_toolbar_action.setChecked(sys_conf.ignore_space)
        self.tree.init_tree_item(self.data)

    def ignore_case(self):
        sys_conf.ignore_case = not sys_conf.ignore_case
        sys_conf.save()
        self.ignore_case_action.setChecked(sys_conf.ignore_case)
        self.ignore_case_toolbar_action.setChecked(sys_conf.ignore_case)
        self.tree.init_tree_item(self.data)

    def ignore_symbol(self):
        sys_conf.ignore_symbol = not sys_conf.ignore_symbol
        sys_conf.save()
        self.ignore_symbol_action.setChecked(sys_conf.ignore_symbol)
        self.ignore_symbol_toolbar_action.setChecked(sys_conf.ignore_symbol)
        self.tree.init_tree_item(self.data)

    # 定义一个关闭窗口时的对话框，如确定退出，则关闭conn的连接。
    def closeEvent(self, event):  # 关闭窗口触发以下事件
        a = QMessageBox.question(self, '退出', '确定要退出吗?', QMessageBox.Yes | QMessageBox.No, QMessageBox.No)
        if a == QMessageBox.Yes:
            event.accept()  # 接受关闭事件
        else:
            event.ignore()  # 忽略关闭事件


def qtmain():
    app = QApplication(sys.argv)
    mainWindow = MainWindow()
    app.setStyleSheet(qdarkstyle.load_stylesheet(qt_api='pyside6', palette=qdarkstyle.DarkPalette))

    mainWindow.show()
    sys.exit(app.exec())

