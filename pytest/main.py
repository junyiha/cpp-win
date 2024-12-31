import os

def get_file_sizes(directory):
    file_sizes = {}
    for root, dirs, files in os.walk(directory):
        for file in files:
            file_path = os.path.join(root, file)
            file_sizes[file_path] = os.path.getsize(file_path)
    return file_sizes

def main():
    directory = 'D:/junyiha.github.io/source/_posts/notes/PDF'
    file_sizes = get_file_sizes(directory)
    sorted_file_sizes = sorted(file_sizes.items(), key=lambda item: item[1], reverse=True)
    for file_path, size in sorted_file_sizes:
        size_mb = size / (1024 * 1024)
        print(f"{file_path}: \n{size_mb:.2f} MB")

if __name__ == "__main__":
    main()
