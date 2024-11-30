#!/bin/bash

clear
printf '\033[44m' # Blue background
printf '\033[2J'  # Clear the screen
printf '\033[H'   # Move cursor to home position

# Config paths
CONFIG_FILE="$CONFIG/config.catk"
BACKUP_FILE="$CATK_ROOT/config.editor.bak"

# Load configuration lines into an array
mapfile -t CONFIG_LINES < "$CONFIG_FILE"

# Variables
current_line=0
display_offset=0
lines_per_screen=$(( $(tput lines) - 8 )) # Adjust for window padding
window_width=60
start_row=$(( ($(tput lines) - lines_per_screen - 4) / 2 ))
start_col=$(( ($(tput cols) - window_width) / 2 ))

# Clear the screen with a blue background
clear_with_background() {
    printf '\033[H' # Move cursor to home position
}

# Draw the window and its border
draw_window_border() {
    local height=$((lines_per_screen + 4))
    # Draw top border
    tput cup $start_row $start_col
    printf "\033[107m\033[30m┌%s┐\033[0m" "$(head -c $((window_width - 2)) < /dev/zero | tr '\0' '─')"
    # Draw side borders
    for ((i = 1; i < height - 1; i++)); do
        tput cup $((start_row + i)) $start_col
        printf "\033[107m\033[30m│\033[0m%*s\033[107m\033[30m│\033[0m" $((window_width - 2)) ""
    done
    # Draw bottom border
    tput cup $((start_row + height - 1)) $start_col
    printf "\033[107m\033[30m└%s┘\033[0m" "$(head -c $((window_width - 2)) < /dev/zero | tr '\0' '─')"
}

# Draw the window shadow
draw_shadow() {
    local shadow_row=$((start_row + lines_per_screen + 4))
    local shadow_col=$((start_col + window_width))

    # Draw bottom shadow (one row)
    tput cup $shadow_row $((start_col + 2))
    printf "\033[100m%*s\033[0m" $((window_width - 2)) ""
    # Draw right shadow (one column)
    for ((i = 0; i <= lines_per_screen + 3; i++)); do
        tput cup $((start_row + i)) $shadow_col
        printf "\033[100m  \033[0m"
    done
}

# Display configuration lines inside the window
display_config() {
    local content_start_row=$((start_row + 2))
    local end_line=$((display_offset + lines_per_screen))

    for ((i = 0; i < lines_per_screen; i++)); do
        local line_index=$((display_offset + i))
        tput cup $((content_start_row + i)) $((start_col + 2))

        if [[ $line_index -ge ${#CONFIG_LINES[@]} ]]; then
            printf "%-*s" $((window_width - 4)) "" # Empty line
        elif [[ $line_index -eq $current_line ]]; then
            printf "\033[44m> \033[97m%-*s\033[0m" $((window_width - 6)) "${CONFIG_LINES[line_index]}"
        else
            printf "  \033[30m%-*s\033[0m" $((window_width - 6)) "${CONFIG_LINES[line_index]}"
        fi
    done
}

# Handle user input
handle_input() {
    local line="${CONFIG_LINES[current_line]}"

    if [[ "$line" =~ (y|n)$ ]]; then
        CONFIG_LINES[current_line]="${line/y/n}" # Toggle y/n
        CONFIG_LINES[current_line]="${line/n/y}"
    elif [[ "$line" =~ \"[^\"]*\" ]]; then
        tput cup $(tput lines) 0
        printf "Enter new string: "
        read -r new_string
        CONFIG_LINES[current_line]=$(echo "$line" | sed -E "s/\"[^\"]*\"/\"$new_string\"/")
    elif [[ "$line" =~ =[[:space:]]*[0-9]+$ ]]; then
        tput cup $(tput lines) 0
        printf "Enter new number: "
        read -r new_number
        CONFIG_LINES[current_line]=$(echo "$line" | sed -E "s/=[[:space:]]*[0-9]+/=$new_number/")
    fi
}

# Move to the next valid line
move_to_next_valid_line() {
    while [[ "${CONFIG_LINES[current_line]}" =~ ^#.*$ ]] || [[ -z "${CONFIG_LINES[current_line]}" ]]; do
        if [[ $1 == "down" ]]; then
            ((current_line++))
        else
            ((current_line--))
        fi

        # Boundary checking
        if [[ $current_line -ge ${#CONFIG_LINES[@]} ]]; then
            current_line=$(( ${#CONFIG_LINES[@]} - 1 ))
            break
        elif [[ $current_line -lt 0 ]]; then
            current_line=0
            break
        fi
    done
}

# Main loop
while true; do
    # Adjust display offset for scrolling
    if [[ $current_line -ge $((display_offset + lines_per_screen)) ]]; then
        display_offset=$((current_line - lines_per_screen + 1))
    elif [[ $current_line -lt display_offset ]]; then
        display_offset=$current_line
    fi

    # Only refresh the necessary parts
    clear_with_background
    draw_window_border
    draw_shadow
    display_config

    # Read a single keypress
    IFS= read -rsn1 key
    case $key in
        $'\x1b') # ESC sequence
            read -rsn2 -t 0.1 key
            if [[ $key == "[A" ]]; then
                ((current_line--))
                move_to_next_valid_line "up"
            elif [[ $key == "[B" ]]; then
                ((current_line++))
                move_to_next_valid_line "down"
            fi
            ;;
        '') # Enter key
            handle_input
            ;;
        q) # Quit on 'q'
            break
            ;;
    esac
done

# Save changes
printf "%s\n" "${CONFIG_LINES[@]}" > "$BACKUP_FILE"
echo -n "Save changes to $CONFIG_FILE? (Y/n): "
read -r save_choice
if [[ "$save_choice" =~ ^[Yy]?$ ]]; then
    cp "$BACKUP_FILE" "$CONFIG_FILE"
    echo "Changes saved to $CONFIG_FILE."
else
    echo "Changes not saved."
fi

clear

