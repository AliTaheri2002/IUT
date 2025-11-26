import random
import time

def generate_minesweeper_map(n, k):
    size = (15, 20) if n == 15 else (n, n)
    map_grid = [[0 for _ in range(size[1])] for _ in range(size[0])] 
    counts = [0] * n
    num = 0

    while num < k:
        x, y = random.randint(0, n - 1), random.randint(0, size[1] - 1)
        if map_grid[y][x] == '★': continue
        counts[x] += 1
        if counts[x] == 4:
            counts[x] -= 1
            continue
        map_grid[y][x] = '★'

        # Update the surrounding cells with mine counts
        for dx, dy in [(-1, 0), (1, 0), (0, -1), (0, 1), (-1, -1), (-1, 1), (1, -1), (1, 1)]:
            if 0 <= x + dx < n and 0 <= y + dy < size[1] and map_grid[y + dy][x + dx] != '★':
                map_grid[y + dy][x + dx] += 1  

        num += 1
    return map_grid

def print_map(map_grid):
    for row in map_grid:
        print(" ".join(str(cell) for cell in row))

def is_won(map_grid):
    return all(cell != '■' for row in map_grid for cell in row)

def ask_rematch(score):
    print(f"Your score: {score}")
    return input("Do you want to try again? (Rematch/Exit): ").lower() == 'rematch'

def minesweeper_game():
    game_status = True
    while game_status:
        size = input("Select Table size (9*9, 12*12, 15*20): ")
        if size == '9*9':
            n, k = 9, 10
        elif size == '12*12':
            n, k = 12, 20
        elif size == '15*20':
            n, k = 15, 40
        else:
            print("Wrong input!")
            continue

        map_grid = generate_minesweeper_map(n, k)
        player_map = [['■' for _ in range(n)] for _ in range(n)]
        score = 0
        total_cells = n * n
        game_end_time = time.time() + (total_cells * 5)

        while time.time() < game_end_time:
            if is_won(player_map):
                print("You have won!")
                print_map(player_map)
                break

            print_map(player_map)
            x, y = int(input("Enter X coordinate: ")) - 1, int(input("Enter Y coordinate: ")) - 1
            flag = input("Do you want a flag? (yes/no): ").lower() == 'yes'

            if flag:
                player_map[y][x] = '⚑'
                if map_grid[y][x] == '★':
                    map_grid[y][x] = '⚑'
                k -= 1
                print(f"Remaining flags: {k}")
            
            if map_grid[y][x] == '★':
                print("Game Over!")
                print_map(map_grid)
                game_status = ask_rematch(score)
                break

            if map_grid[y][x] != '★':
                player_map[y][x] = map_grid[y][x]
                score += 1

            print_map(player_map)

        else:
            print("Time's up!")
            break

def manage_profile():
    try:
        with open("profile.txt", "r") as profile_file:
            profile_data = profile_file.readlines()
            if len(profile_data) == 0:
                name = input("Enter your name: ")
                win, lose = 0, 0
                with open("profile.txt", "w") as profile_file:
                    profile_file.write(f"name:{name}\nwin:0\nlose:0")
                print(f"Welcome {name}!")
            else:
                name = profile_data[0].split(":")[1].strip()
                win = int(profile_data[1].split(":")[1].strip())
                lose = int(profile_data[2].split(":")[1].strip())
                print(f"Welcome back {name}")
                print(f"Win: {win} Lose: {lose}")

            return name, win, lose
    except FileNotFoundError:
        with open("profile.txt", "w") as profile_file:
            profile_file.write("name:Unknown\nwin:0\nlose:0")
        return "Unknown", 0, 0

def save_profile(name, win, lose):
    with open("profile.txt", "w") as profile_file:
        profile_file.write(f"name:{name}\nwin:{win}\nlose:{lose}")

def main():
    profile_name, profile_win, profile_lose = manage_profile()
    profile_ = 0
    while profile_ >= 0:
        order = input("Enter your order (Change Name, Play!, History): ").lower()

        if order == "change name":
            new_name = input("Enter your name: ")
            save_profile(new_name, profile_win, profile_lose)

        elif order == "history":
            print(f"Win: {profile_win} Lose: {profile_lose}")
            with open('fifi.txt', 'r') as file:
                print(file.read())

        elif order == "play!":
            minesweeper_game()
            save_profile(profile_name, profile_win, profile_lose)
            profile_ = 0

if __name__ == "__main__":
    password = int(input("Enter your password: "))
    if password == 123003:
        main()
    else:
        print("Suspected fraud.")
