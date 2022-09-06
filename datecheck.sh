#!/bin/sh

if [ $# -ne 3 ]; then
	echo "BAD INPUT: wrong number of arguments; 3 needed (month day year)"
	exit
fi

check_year()
{
    YEAR=$1
    if (( $YEAR % 4 == 0 )); then
        if (( $YEAR % 100 == 0 )); then
            if (( $YEAR % 400 == 0 )); then
                return 1
            fi
        else
            return 1
        fi
    fi
    return 2
}

MONTHS="Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec"

MONTH=$1
DAY=$2
YEAR=$3

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
    MONTH=$(echo $MONTH | tr '[:upper:]' '[:lower:]')   
    MONTH=$(echo $MONTH | sed -e "s/\b./\u\0/g")
fi

if [ ! $(echo $DAY | grep -E '^[[:digit:]]+$') ]; then
    echo "BAD INPUT: Input day is not a number"
    exit 1 
fi

if [ ! $(echo $YEAR | grep -E '^[[:digit:]]+$') ]; then
    echo "BAD INPUT: Input year is not a number"
    exit 1
fi

case "${MONTH}" in
    Jan|Mar|May|Jul|Aug|Oct|Dec)
        if [ $DAY -gt 31 ]; then
            echo "BAD INPUT: $MONTH does not have $DAY days."    
            exit 1
        fi
        ;;
    Apr|Jun|Sep|Nov)
        if [ $DAY -gt 30 ]; then
            echo "BAD INPUT: $MONTH does not have $DAY days."    
            exit 1
        fi
        ;;
    Feb)
        check_year $YEAR
        YEAR_RETURN=$?
        if [ $YEAR_RETURN == 1 ];
        then
            if [ $DAY -gt 29 ]; then 
                echo "BAD INPUT: $MONTH $YEAR does not have $DAY days."
                exit 1
            fi
        else
            if [ $DAY -gt 28 ]; then
                echo "BAD INPUT: $MONTH $YEAR does not have $DAY days: not a leap year."
                exit 1
            fi
        fi
        ;;
    *)
        echo "BAD INPUT: $1 is not a month that exists."
        exit 1
        ;;
esac
echo "EXISTS! $MONTH $DAY $YEAR is someone's birthday!"