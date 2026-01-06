import os
import sys

def rename_shaders(root_dir):
    """
    递归修改目录中所有以 1.shader 结尾的文件为 .cg 扩展名
    """
    renamed_count = 0
    error_count = 0

    for root, dirs, files in os.walk(root_dir):
        for filename in files:
            # 检查文件后缀是否符合要求
            if filename.endswith(".reflect"):
                old_path = os.path.join(root, filename)
                
                try:
                    # 构建新文件名（保留原前缀）
                    new_filename = filename[:-len(".reflect")] + ".cache"
                    new_path = os.path.join(root, new_filename)
                    
                    # 执行重命名
                    os.rename(old_path, new_path)
                    print(f"Renamed: {old_path} -> {new_path}")
                    renamed_count += 1
                    
                except Exception as e:
                    error_count += 1
                    print(f"Error renaming {old_path}: {str(e)}", file=sys.stderr)

    print(f"\n操作完成：成功 {renamed_count} 个，失败 {error_count} 个")

if __name__ == "__main__":
    
    target_dir = "./"
    
    if not os.path.isdir(target_dir):
        print(f"错误：{target_dir} 不是有效目录")
        sys.exit(2)
    
    rename_shaders(target_dir)