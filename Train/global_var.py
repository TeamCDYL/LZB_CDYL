# 全局变量管理

def _init():
    # 初始化
    global _global_dict
    _global_dict = {}


def set_value(name, value):
    # 设定值
    _global_dict[name] = value


def get_value(name, defValue=None):
    # 获取值
    try:
        return _global_dict[name]
    except KeyError:
        return defValue
