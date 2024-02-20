# -*- coding: utf-8 -*-
"""
@author     : mjsaka
@contact    : qiuxuenan@gmail.com
@time       : 2024/2/6 9:03
@file       : mssql.py
@description: Connects to a SQL database using pymssql
"""

import traceback
import pymssql


NameField = 'FName'
ModelField = 'FModel'

_conn = None


def con():
	global _conn
	server='192.168.1.254'
	user='KISAdmin'
	password='ypbwkfyjhyhgzj'
	database='AIS20170411161100'
	#database='AIS20170411161221'

	if not _conn:
		try:
			_conn = pymssql.connect(server=server, user=user, password=password, database=database, as_dict=True, timeout=3)
		except Exception as e:
			_conn = None
			traceback.print_exc()

	return _conn


def records_conv(records):
	new_records = []

	if len(records) < 1:
		return new_records

	charset='latin1'
	fields = list(records[0])
	
	for row in records:
		new_row = {}
		
		for field in fields:
			new_row[field] = row[field]
			
			if type(row[field]) is str:
				new_row[field] = row[field].encode(charset).decode('gbk')
		
		new_records.append(new_row)
	return new_records


def get_records(name, model):
	SQL_QUERY = f"""
		SELECT {NameField}, {ModelField}
		FROM t_ICItemCore
		WHERE {ModelField} = %s
	"""

	records1 = [{NameField: '数据库操作失败', ModelField: model}]

	conn = con()

	if conn:
		try:
			cursor = conn.cursor()
			cursor.execute(SQL_QUERY, model)

			records0 = cursor.fetchall()
			records1 = records_conv(records0)
		except Exception as e:
			global _conn
			_conn = None
			traceback.print_exc()

	return records1


def get_tables():
	SQL_QUERY = f"""
			SELECT FTableName
			FROM t_TableDescription
			WHERE FDescription like %s
		"""

	conn = con()
	cursor = conn.cursor()
	cursor.execute(SQL_QUERY, '%账%')

	records0 = cursor.fetchall()
	records1 = records_conv(records0)

	tables = []
	for row in records1:
		tables.append(row['FTableName'])

	return tables


def get_data(table):
	SQL_QUERY = f"""
				SELECT top 100 *
				FROM {table}
			"""

	conn = con()
	cursor = conn.cursor()
	cursor.execute(SQL_QUERY)

	records0 = cursor.fetchall()
	records1 = records_conv(records0)

	return records1


def get_datas():
	tables = get_tables()

	datas = {}
	for table in tables:
		datas[table] = get_data(table)
	return datas


