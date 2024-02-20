import json
from pathlib import Path

__all__ = ['Conf', 'ConfItem']


#################### 对象输出辅助 ####################
def listobj2str(listobj, indent=''):
    info = ''
    for obj in listobj:
        if isinstance(obj, Conf):
            info += confobj2str(obj, indent)
        else:
            info += f'{obj}'
    return info


def dictobj2str(dictobj, indent=''):
    info = ''
    for key, obj in dictobj.items():
        if isinstance(obj, Conf):
            info += f'\n{indent}{key}:'
            info += confobj2str(obj, indent + '  ')
        elif isinstance(obj, list):
            info += f'\n{indent}{key}:['
            info += listobj2str(obj, indent + '  ')
            info += f'\n{indent}]'
        else:
            info += f'\n{indent}{key}: {obj}'
    return info


def confobj2str(obj, indent=''):
    info = '\n' + indent
    info += obj.__class__.__name__ + '{'
    info += dictobj2str(obj.__dict__, indent + '  ')
    info += '\n' + indent + '}'
    return info


#################### JSON转换辅助 ####################
def obj2dictobj(obj):
    if isinstance(obj, Conf):
        return obj.__dict__
    return json.JSONEncoder().default(obj)


def dictobj2confobj(dictobj):
    return Conf(dictobj)


#################### 对象复制辅助 ####################
def copylistobj(fromlistobj, tolistobj):
    fromlen = len(fromlistobj)
    tolen = len(tolistobj)
    copylen = fromlen if fromlen < tolen else tolen
    for i in range(copylen):
        if isinstance(fromlistobj[i], Conf) and isinstance(tolistobj[i], Conf):
            copyconfobj(fromlistobj[i], tolistobj[i])
        else:
            tolistobj[i] = fromlistobj[i]


def copyconfobj(fromconfobj, toconfobj):
    for k, vt in toconfobj.items():
        vf = getattr(fromconfobj, k)
        if vf is None:
            continue

        if isinstance(vf, Conf) and isinstance(vt, Conf):
            copyconfobj(vf, vt)
        elif isinstance(vf, list) and isinstance(vt, list):
            copylistobj(vf, vt)
        elif type(vf) == type(vt):
            toconfobj[k] = vf


class Conf(object):
    """ base class of configure process
    each configure is an instance of Conf class
    each configure has several item attributes which can be three types:
    1, base type object such as int, float, bool, string
    2, Conf object
    3, list of Conf object
    """
    def __init__(self, params={}):
        super().__init__()
        for k, v in params.items():
            self.__setattr__(k, v)

    def __str__(self):
        return self.dumps()
        return confobj2str(self)

    def __repr__(self):
        return self.dumps()
        return confobj2str(self)

    def __getattr__(self, item):
        if item in self.__dict__:
            return self.__dict__[item]
        return None

    def __setattr__(self, attr, value):
        self.__dict__[attr] = value

    #################### 容器模拟 ####################
    def __getitem__(self, item):
        return self.__getattr__(item)

    def __setitem__(self, key, value):
        return self.__setattr__(key, value)

    def __len__(self):
        return len(self.__dict__)

    def __iter__(self):
        return self.__dict__.__iter__()

    def __contains__(self, item):
        return self.__dict__.__contains__(item)

    #################### dict模拟 ####################
    def get(self, key):
        return self.__getattr__(key)

    def keys(self):
        return self.__dict__.keys()

    def values(self):
        return self.__dict__.values()

    def items(self):
        return self.__dict__.items()

    #################### 导入导出 ####################
    def dumps(self):
        jstr = json.dumps(self.__dict__, ensure_ascii=False, indent=4, default=obj2dictobj)
        return jstr

    def loads(self, jstr):
        obj = json.loads(jstr, object_hook=dictobj2confobj)
        copyconfobj(obj, self)

    def dump(self, path):
        with open(path, 'w', encoding='utf8') as f:
            jstr = self.dumps()
            f.write(jstr)

    def load(self, path):
        if not Path(path).exists():
            return

        with open(path, 'r', encoding='utf8') as f:
            jstr = f.read()
            self.loads(jstr)


class ConfItem(Conf):
    def __init__(self, name, value):
        super().__init__()
        self.name = name
        self.value = value

