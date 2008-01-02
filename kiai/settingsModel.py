#!/usr/bin/env python
from PyQt4 import QtCore
def debug(x): print x
class settingItem:
	#TODO: we could store whether a node is a leaf, and thus provide an efficient implementation of settingsModel.hasChildren()
	def __init__(self,path,selfindex,parent,data,column=0):
		self.path=path
		self.selfindex=selfindex
		self.parent=parent
		if(data):
			self.data=data
		else:
			self.data=path[-1]
		self.children=()
		self.column=column
	def rowCount(self): return len(self.children)
	def index(self,row,column):
		if(row<0 or row>len(self.children) or column <0 or column >2): return QtCore.QModelIndex()
		else:
			path=self.path+(self.children[row],)
			try:
				r=self.selfindex.model().indexof(path,column)
				return self.selfindex.model().createIndex(row,column,r)
			except Exception:
				if(column==2): return QModelIndex()
				self.selfindex.model().l.append(settingItem(path,None,self.selfindex,None))
				i=self.selfindex.model().createIndex(row,column,self.selfindex.model().indexof(path,column))
				self.selfindex.model().l[i.internalId()].selfindex=i
				self.selfindex.model().emit(QtCore.SIGNAL("requestSetting(PyQt_PyObject)"),path)
				return i

class settingsModel(QtCore.QAbstractItemModel):
	def __init__(self,parent=None):
		QtCore.QAbstractItemModel.__init__(self,parent)
		self.l=[settingItem(('',),self.createIndex(0,0,0),QtCore.QModelIndex(),"[root]")]
	def data(self,index,role):
		if((role!=QtCore.Qt.DisplayRole()) or not index.isValid()): return QVariant()
		else: return QVariant(self.l[index.internalId()].data)
	def parent(self,index):
		if(not index.isValid()): return QtCore.QModelIndex()
		else:	
			debug("returning index "+`self.l[index.internalId()].parent`+" from parent()")
			return self.l[index.internalId()].parent
	def rowCount(self,index):
		if(not index.isValid()): return 1
		else: return self.l[index.internalId()].rowCount()
	def columnCount(self,index): return 2
	def index(self,index,row,column):
		debug("settingsModel.index called with (parent) index "+`index`)
		if(not index.isValid()): 
			if(row==0 and column==0):
				return self.createIndex(0,0,0)
			else:
				return QtCore.QModelIndex()
		else:
			debug("returning index "+`self.l[index.internalId()].index(row,column)`+" from index()")
			return self.l[index.internalId()].index(row,column)
	def indexof(self,path,column):
		for i in range(len(self.l)):
			if(self.l[i].path==path and self.l[i].column==column): return i
		print "Error: could not find an entry for column "+`column`+" and path "+`path`+" in list "+`self.l`
		raise Exception()
	def headerData(self,section,orientation,role):
		if(role==QtCore.Qt.DisplayRole and orientation==QtCore.Qt.Horizontal and 0<=section and 2>=section):
			if(section==0):
				return QtCore.QVariant("Node")
			else:
				return QtCore.QVariant("Data")
		else:
			return QtCore.QVariant()
	def processUpdate(self,d):
		path=tuple(d.getSetting().split(':'))
		debug("working on path "+`path`)
		n=self.indexof(path,0)
		if(not d.isLeaf()):
			self.beginInsertRows(self.l[self.indexof(path,0)].selfindex,0,len(d.getValue())-1)
			self.l[n].children=d.getValue()
			self.endInsertRows()
		else:
			try:
				m=self.indexof(path,1)
			except Exception:
				self.beginInsertColumns()
				m=settingItem(path,None,n.parent,d.getValue()[0],1)
				i=self.indexof(path,1)
				self.l[i].selfindex=self.createIndex(n.selfindex.row(),1,i)