#!/bin/bash

# Set the configuration file paths
CONFIG_FILE="$CONFIG/config.catk"
BACKUP_FILE="$CATK_ROOT/config.editor.bak"

# Load the configuration into an array
mapfile -t CONFIG_LINES < "$CONFIG_FILE"

# Initialize current line and display offset
current_line=0
display_offset=0
lines_per_screen=$(($(tput lines) - 1))

clear_with_color() {
    clear
    # Set the background color to white and fill the screen
    printf '\033[47m'   # Set background color to white
    printf '\033[2J'    # Clear the screen
    printf '\033[H'     # Move cursor to home position (top-left)
}

clear_with_color

display_config() {
    tput cup 0 0  # Move cursor to the top left corner without clearing screen
    local end_line=$((display_offset + lines_per_screen))

    for i in "${!CONFIG_LINES[@]}"; do
        # Skip lines outside the current display range
        if [[ $i -lt $display_offset ]] || [[ $i -ge $end_line ]]; then
            continue
        fi
        
        if [[ "${CONFIG_LINES[i]}" =~ ^#.*$ ]] || [[ -z "${CONFIG_LINES[i]}" ]]; then
            continue
        fi
        
        # Add extra newlines around headers
        if [[ "${CONFIG_LINES[i]}" =~ ^%.*$ ]]; then
            echo ""
            echo -e "\e[1m\e[90m\e[47m${CONFIG_LINES[i]}\e[0m"
            echo ""
            continue
        fi
        
        if [[ $i -lt $display_offset ]] || [[ $i -ge $end_line ]]; then
            continue
        fi
        
        if [[ $i -eq $current_line ]]; then
            echo -e "\e[37m\e[44m> \e[97m${CONFIG_LINES[i]}\e[0m"  # Highlight the current line with "> "
        else
            echo -e "\e[30m\e[47m  ${CONFIG_LINES[i]}\e[0m" 
        fi
    done
}

# Function to handle user input
handle_input() {
    local line="${CONFIG_LINES[current_line]}"
    
    if [[ "$line" =~ (y|n)$ ]]; then
        if [[ "$line" == *y ]]; then
            CONFIG_LINES[current_line]="${line/y/n}"
        else
            CONFIG_LINES[current_line]="${line/n/y}"
        fi
    elif [[ "$line" =~ \"[^\"]*\" ]]; then
        echo -n "Enter new string: "
        read -r new_string
        CONFIG_LINES[current_line]=$(echo "$line" | sed -E "s/\"[^\"]*\"/\"$new_string\"/")
    elif [[ "$line" =~ =[[:space:]]*[0-9]+$ ]]; then
        echo -n "Enter new number: "
        read -r new_number
        CONFIG_LINES[current_line]=$(echo "$line" | sed -E "s/=[[:space:]]*[0-9]+/=$new_number/")
    fi
}

# Skip comments, empty lines, and headers when moving
move_to_next_valid_line() {
    clear_with_color
    while [[ "${CONFIG_LINES[current_line]}" =~ ^#.*$ ]] || [[ -z "${CONFIG_LINES[current_line]}" ]] || [[ "${CONFIG_LINES[current_line]}" =~ ^%.*$ ]]; do
        if [[ $1 == "down" ]]; then
            ((current_line++))
        else
            ((current_line--))
        fi
        
        # Bound checking
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
    elif [[ $current_line -lt $display_offset ]]; then
        display_offset=$current_line
    fi

    display_config
    
    # Read a single keypress
    IFS= read -rsn1 key
    case $key in
        $'\x1b')  # ESC sequence
            read -rsn2 -t 0.1 key  # Read 2 more characters
            if [[ $key == "[A" ]]; then
                ((current_line--))
                move_to_next_valid_line "up"
            elif [[ $key == "[B" ]]; then
                ((current_line++))
                move_to_next_valid_line "down"
            fi
            ;;
        '')  # Enter key
            handle_input
            ;;
        q)  # Quit on 'q'
            break
            ;;
    esac
done

# Save the backup file
printf "%s\n" "${CONFIG_LINES[@]}" > "$BACKUP_FILE"

echo -n "Save changes to $CONFIG_FILE? (Y/n): "
read -r save_choice
if [[ "$save_choice" =~ ^[Yy]?$ ]]; then
    cp "$BACKUP_FILE" "$CONFIG_FILE"
    echo "Changes saved to $CONFIG_FILE."
    # clean up
    rm $BACKUP_FILE
else
    echo "Changes not saved."
    # clean up
    rm $BACKUP_FILE
fi

clear
