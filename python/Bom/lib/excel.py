# -*- coding: utf-8 -*-
"""
@author     : mjsaka
@contact    : qiuxuenan@gmail.com
@time       : 2024/2/6 9:03
@file       : excel.py
@description: 
"""
import json

from collections import OrderedDict

from lib.crypt import DecryptFile

from lib.mssql import get_records
from lib.mssql import ModelField
from lib.mssql import NameField

from PySide6.QtCore import (Signal, QThread)

from pathlib import Path
import xlwings


class ExcelChecker(QThread):
    # 进度信号
    progress_signal = Signal(dict)
    finish_signal = Signal(dict)

    def __init__(self, file_path):
        super().__init__()

        self.header_row = 7
        self.name_col = 1
        self.model_col = 2

        print('check_excel_file:', file_path)
        # 文件处理
        suffix = Path(file_path).suffix
        tempdir = Path('c:/windows/temp/')
        filename = '7806b644-6e1c-42f9-9d42-7479cc776dc6'
        self.file_path = tempdir.joinpath(filename + suffix)

        DecryptFile(file_path, self.file_path)

    def run(self):
        data = self.check_excel()
        self.finish_signal.emit(data)

    def check_excel_header(self, sheet):
        name = sheet.range(self.header_row, self.name_col).value
        model = sheet.range(self.header_row, self.model_col).value

        print('check_excel_header:', name, model)

        if not name or type(name) != str or name.find('名称') < 0:
            return False

        if not model or type(model) != str or (model.find('图号') < 0 and model.find('型号') < 0):
            return False

        return True

    def get_row_data(self, sheet, row):
        name_cell = sheet.range(row, self.name_col)
        model_cell = sheet.range(row, self.model_col)
        name = name_cell.value
        model = model_cell.value
        sname = str(name)
        smodel = str(model)

        print('get_row_data:', row, name, model)

        if name is None:
            return None

        if name_cell.merge_area == model_cell.merge_area:
            return {'type': 'section', 'name': sname, 'model': '', 'records': []}

        if model is None:
            return None

        if model_cell.number_format == 'G/通用格式':
            if type(model_cell.value) == type(float):
                smodel = str(int(model_cell.value))

        return{'type': 'data', 'name': sname, 'model': smodel, 'records': []}

    def get_sheet_data(self, sheet):
        print('get_sheet_data:', sheet.name)

        # 检查表头
        if not self.check_excel_header(sheet):
            return None

        sheet_data = OrderedDict()
        # 获取每行数据
        rows = sheet.used_range.rows.count
        for row in range(self.header_row + 1, rows + 1):
            signal_info = {'info': f'加载中...{sheet.name}-{row}/{rows}',
                           'total': rows,
                           'current': row}
            self.progress_signal.emit(signal_info)

            row_data = self.get_row_data(sheet, row)
            if row_data:
                sheet_data[str(row)] = row_data

        return sheet_data

    def get_excel_data(self):
        # 打开excel app
        app = xlwings.App(visible=False, add_book=False)
        app.display_alerts = False
        app.screen_updating = False

        # 打开excel文件
        book = app.books.open(self.file_path)

        book_data = OrderedDict()
        #获取每个表格数据
        for sheet in book.sheets:
            sheet_data = self.get_sheet_data(sheet)
            if sheet_data:
                book_data[sheet.name] = sheet_data

        # 关闭excel文件 退出excel app
        book.close()
        app.quit()

        return book_data

    def check_row(self, row, row_data):
        if row_data['type'] != 'data':
            return

        name = row_data['name']
        model = row_data['model']

        print('check_row:', row, name, name.encode('utf8').hex(), model, model.encode('utf8').hex())

        records = get_records(name, model)
        for record in records:
            rname = record[NameField]
            rmodel = record[ModelField]
            row_data['records'].append({'name': rname, 'model': rmodel})
            print('    record:', rname, rname.encode('utf8').hex(), rmodel, rmodel.encode('utf8').hex())

    def check_sheet(self, sheet, sheet_data):
        print('check_sheet:', sheet)

        for row_index, (row, row_data) in enumerate(sheet_data.items()):
            name = row_data['name']
            model = row_data['model']
            signal_info = {'info': f'核对中...{sheet}-{name}-{model}',
                           'total': len(sheet_data),
                           'current': row_index}
            self.progress_signal.emit(signal_info)

            self.check_row(row, row_data)

    def check_excel(self):
        data = self.get_excel_data()

        for sheet, sheet_data in data.items():
            self.check_sheet(sheet, sheet_data)

        with open('data.json', 'w', encoding='utf8') as f:
            json.dump(data, f, ensure_ascii=False, indent=2)

        return data







