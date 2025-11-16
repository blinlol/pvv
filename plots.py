import os
import re
import pandas as pd
from pathlib import Path

def parse_results_directory(results_dir="results"):
    """
    Парсит все файлы в директории results и извлекает время выполнения solve
    """
    data = []
    
    # Регулярное выражение для извлечения времени выполнения solve
    solve_pattern = re.compile(r'solve executed in ([\d.]+)s')
    
    # Регулярное выражение для парсинга имени файла
    filename_pattern = re.compile(r'^([a-zA-Z]+)-(\d+)-(\d+)(?:\.\d+)?$')
    
    results_path = Path(results_dir)
    
    if not results_path.exists():
        print(f"Директория {results_dir} не существует!")
        return pd.DataFrame()
    
    for file_path in results_path.iterdir():
        if not file_path.is_file():
            continue
            
        # Парсим имя файла
        match = filename_pattern.match(file_path.stem)
        if not match:
            print(f"Неверный формат имени файла: {file_path.name}")
            continue
            
        name, N_str, t_str = match.groups()
        N = int(N_str)
        t = int(t_str)
        
        # Читаем файл и ищем время выполнения solve
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                content = f.read()
                
            # Ищем время выполнения solve
            solve_match = solve_pattern.search(content)
            if solve_match:
                solve_time = float(solve_match.group(1))
                
                data.append({
                    'name': name,
                    'N': N,
                    'threads': t,
                    'solve_time': solve_time
                })
                print(f"Обработан {file_path.name}: solve_time = {solve_time}s")
            else:
                print(f"В файле {file_path.name} не найдено время выполнения solve")
                
        except Exception as e:
            print(f"Ошибка при чтении файла {file_path.name}: {e}")
    
    # Создаем DataFrame
    df = pd.DataFrame(data)
    
    # Сортируем для удобства
    if not df.empty:
        df = df.sort_values(['name', 'N', 'threads']).reset_index(drop=True)
    
    return df

def create_summary_tables(df):
    """
    Создает сводные таблицы для анализа
    """
    if df.empty:
        return {}
    
    tables = {}
    
    # Таблица: время выполнения по N и количеству потоков для каждого name
    for name in df['name'].unique():
        name_df = df[df['name'] == name]
        pivot_table = name_df.pivot_table(
            values='solve_time',
            index='N',
            columns='threads',
            aggfunc='mean'
        )
        tables[f'{name}_pivot'] = pivot_table
        print(f"\nСводная таблица для {name}:")
        print(pivot_table)
    
    return tables

def save_results(df, output_file="results_analysis.csv"):
    """
    Сохраняет результаты в CSV файл
    """
    if not df.empty:
        df.to_csv(output_file, index=False)
        print(f"\nРезультаты сохранены в {output_file}")

def main():
    print("Начинаем парсинг результатов...")
    
    # Парсим результаты
    df = parse_results_directory("results")
    
    if df.empty:
        print("Не найдено данных для анализа")
        return
    
    print(f"\nУспешно обработано {len(df)} записей")
    print("\nПервые 10 строк DataFrame:")
    print(df.head(10))
    
    # Создаем сводные таблицы
    tables = create_summary_tables(df)
    
    # Сохраняем результаты
    save_results(df)
    
    # Базовая статистика
    print("\nСтатистика по времени выполнения:")
    print(df.groupby('name')['solve_time'].describe())
    
    return df

import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np

def create_plots(df, output_dir="plots"):
    """
    Создает графики на основе проанализированных данных
    """
    if df.empty:
        print("Нет данных для построения графиков")
        return
    
    # Создаем директорию для графиков
    Path(output_dir).mkdir(exist_ok=True)
    
    # Настройка стиля
    plt.style.use('seaborn-v0_8')
    sns.set_palette("husl")
    
    # 1. График: время выполнения vs количество потоков для разных N
    for name in df['name'].unique():
        name_df = df[df['name'] == name]
        
        plt.figure(figsize=(12, 8))
        
        for N in sorted(name_df['N'].unique()):
            N_data = name_df[name_df['N'] == N]
            plt.plot(N_data['threads'], N_data['solve_time'], 
                    marker='o', linewidth=2, markersize=8, label=f'N={N}')
        
        plt.xlabel('Количество потоков')
        plt.ylabel('Время выполнения (сек)')
        plt.title(f'Производительность {name}: Время выполнения vs Потоки')
        plt.legend()
        plt.grid(True, alpha=0.3)
        plt.xscale('log', base=2)
        plt.yscale('log')
        
        plt.tight_layout()
        plt.savefig(f'{output_dir}/{name}_threads_vs_time.png', dpi=300, bbox_inches='tight')
        plt.close()
    
    # 2. График: время выполнения vs размер задачи для разных количеств потоков
    for name in df['name'].unique():
        name_df = df[df['name'] == name]
        
        plt.figure(figsize=(12, 8))
        
        for threads in sorted(name_df['threads'].unique()):
            thread_data = name_df[name_df['threads'] == threads]
            plt.plot(thread_data['N'], thread_data['solve_time'], 
                    marker='s', linewidth=2, markersize=6, label=f'{threads} потоков')
        
        plt.xlabel('Размер задачи (N)')
        plt.ylabel('Время выполнения (сек)')
        plt.title(f'Масштабируемость {name}: Время выполнения vs Размер задачи')
        plt.legend()
        plt.grid(True, alpha=0.3)
        plt.xscale('log', base=2)
        plt.yscale('log')
        
        plt.tight_layout()
        plt.savefig(f'{output_dir}/{name}_N_vs_time.png', dpi=300, bbox_inches='tight')
        plt.close()
    
    # 3. Heatmap: тепловая карта производительности
    for name in df['name'].unique():
        name_df = df[df['name'] == name]
        
        # Создаем сводную таблицу для heatmap
        pivot_data = name_df.pivot_table(
            values='solve_time',
            index='N',
            columns='threads',
            aggfunc='mean'
        )
        
        plt.figure(figsize=(10, 8))
        sns.heatmap(pivot_data, annot=True, fmt='.2f', cmap='YlOrRd_r',
                   cbar_kws={'label': 'Время выполнения (сек)'})
        plt.title(f'Тепловая карта производительности {name}')
        plt.tight_layout()
        plt.savefig(f'{output_dir}/{name}_heatmap.png', dpi=300, bbox_inches='tight')
        plt.close()
    
    # 4. График ускорения (speedup)
    for name in df['name'].unique():
        name_df = df[df['name'] == name]
        
        plt.figure(figsize=(12, 8))
        
        for N in sorted(name_df['N'].unique()):
            N_data = name_df[name_df['N'] == N].sort_values('threads')
            
            # Вычисляем ускорение относительно 1 потока
            single_thread_time = N_data[N_data['threads'] == 1]['solve_time']
            if len(single_thread_time) > 0:
                base_time = single_thread_time.values[0]
                speedup = base_time / N_data['solve_time']
                
                plt.plot(N_data['threads'], speedup, 
                        marker='o', linewidth=2, markersize=8, label=f'N={N}')
        
        # Идеальное ускорение
        max_threads = max(name_df['threads'])
        ideal_x = [1, max_threads]
        ideal_y = [1, max_threads]
        plt.plot(ideal_x, ideal_y, 'k--', alpha=0.5, label='Идеальное ускорение')
        
        plt.xlabel('Количество потоков')
        plt.ylabel('Ускорение')
        plt.title(f'Ускорение {name} относительно 1 потока')
        plt.legend()
        plt.grid(True, alpha=0.3)
        plt.xscale('log', base=2)
        
        plt.tight_layout()
        plt.savefig(f'{output_dir}/{name}_speedup.png', dpi=300, bbox_inches='tight')
        plt.close()
    
    print(f"Графики сохранены в директории {output_dir}")

# Обновленная main функция с графиками
def main_with_plots():
    print("Начинаем анализ результатов...")
    
    # Парсим результаты
    df = parse_results_directory("results")
    
    if df.empty:
        print("Не найдено данных для анализа")
        return
    
    print(f"\nУспешно обработано {len(df)} записей")
    
    # Создаем сводные таблицы
    tables = create_summary_tables(df)
    
    # Сохраняем результаты
    save_results(df)
    
    # Создаем графики
    create_plots(df)
    
    return df

if __name__ == "__main__":
    df = main_with_plots()
