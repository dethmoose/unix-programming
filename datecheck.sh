#!/bin/sh

# TODO handle long month names

error() { echo $1; exit 1; }

if [ $# -ne 3 ]; then
	error "BAD INPUT: wrong number of arguments; 3 needed (month day year)"
fi

MONTHS="Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec"

MONTH=$1
DAY=$2
YEAR=$3

check_leapyear()
{
    YEAR=$1
    if [ $(expr $YEAR % 4) -eq 0 ]; then
        if [ $(expr $YEAR % 100) -eq 0 ]; then
            if [ $(expr $YEAR % 400) -eq 0 ]; then
                return 1
            fi
        else
            return 1
        fi
    fi
    return 2
}

# Handle month input (number or letters)
if [ "$(echo $MONTH | grep -E '^[[:digit:]]+$')" ]; then
    count=0
    for VARIABLE in $MONTHS
    do
        count=`expr $count + 1`
        if [ $count -eq $MONTH ]; then
        MONTH=$VARIABLE
            break
        fi
    done
else
    MONTH=$(echo $MONTH | tr '[:upper:]' '[:lower:]')   # to lowercase  
    MONTH=$(echo $MONTH | sed -e "s/\b./\u\0/g")        # uppercase first char
fi

if [ ! $(echo $DAY | grep -E '^[[:digit:]]+$') ]; then
    error "BAD INPUT: Input day is not a number"
fi

if [ ! $(echo $YEAR | grep -E '^[[:digit:]]+$') ]; then
    error "BAD INPUT: Input year is not a number"
fi

# Validate day for the chosen month
case "${MONTH}" in
    Jan|Mar|May|Jul|Aug|Oct|Dec)
        if [ $DAY -gt 31 ]; then
            error "BAD INPUT: $MONTH does not have $DAY days."    
        fi
        ;;
    Apr|Jun|Sep|Nov)
        if [ $DAY -gt 30 ]; then
            error "BAD INPUT: $MONTH does not have $DAY days."    
        fi
        ;;
    Feb)
        check_leapyear $YEAR
        LEAPYEAR_RETURN=$?
        if [ $LEAPYEAR_RETURN -eq 1 ] && [ $DAY -gt 29 ]; then
            error "BAD INPUT: $MONTH $YEAR does not have $DAY days."
        elif [ $LEAPYEAR_RETURN -ne 1 ] && [ $DAY -gt 28 ]; then
            error "BAD INPUT: $MONTH $YEAR does not have $DAY days: not a leap year."
        fi
        ;;
    *)
        error "BAD INPUT: $1 is not a month that exists."
        ;;
esac

# Print valid date
echo "EXISTS! $MONTH $DAY $YEAR is someone's birthday!"
