import os
import re
from pathlib import Path

def convert_variable_name(match):
    name = match.group(1)
    # 排除以vs_或ps_开头的变量
    if name.startswith(('vs_', 'ps_')):
        return name
    # 排除全大写下划线的变量（常量）
    if name.isupper() and '_' in name:
        return name
    # 转换小驼峰
    parts = name.split('_')
    return parts[0].lower() + ''.join(x.capitalize() for x in parts[1:])

def process_file_content(content):
    pattern = re.compile(r'\b([A-Za-z_][A-Za-z0-9_]*)\b')
    return pattern.sub(convert_variable_name, content)

def process_directory(root_dir):
    for root, dirs, files in os.walk(root_dir):
        for file in files:
            if Path(file).suffix.lower() in ('.h', '.cpp'):
                file_path = os.path.join(root, file)
                try:
                    with open(file_path, 'r+', encoding='utf-8') as f:
                        content = f.read()
                        modified = process_file_content(content)
                        if modified != content:
                            f.seek(0)
                            f.write(modified)
                            f.truncate()
                            print(f"Processed: {file_path}")
                except Exception as e:
                    print(f"Error processing {file_path}: {str(e)}")

if __name__ == "__main__":
    # 使用前请备份重要文件！
    target_dir = input("请输入要处理的目录路径：")
    if os.path.isdir(target_dir):
        process_directory(target_dir)
        print("处理完成")
    else:
        print("无效的目录路径")