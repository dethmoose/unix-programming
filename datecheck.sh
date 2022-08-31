
check_year()
{
    YEAR=$1
    if (( $YEAR % 4 == 0 ));
    then
        if (( $YEAR % 100 == 0 ));
        then
            if (( $YEAR % 400 == 0 ));
            then
                return 1
            fi
        else
            return 1
        fi
    fi
    return 2
}

MONTHS=(Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec)


MONTH=${1,,}
DAY=$2
YEAR=$3

if [[ $MONTH =~ ^[0-9]+$ ]]
then
    MONTH=${MONTHS[$1-1],,}
fi

if ! [[ $DAY =~ ^[0-9]+$ ]]
then
    echo "BAD INPUT: Input day is not a number"
    exit 1
fi

if ! [[ $YEAR =~ ^[0-9]+$ ]]
then
    echo "BAD INPUT: Input year is not a number"
    exit 1
fi

case "${MONTH}" in
    jan|mar|may|jul|aug|oct|dec)
        if [ $DAY -gt 31 ]; then
            echo "BAD INPUT: ${MONTH^} does not have $DAY days."    
            exit 1
        fi
        ;;
    apr|jun|sep|nov)
        if [ $DAY -gt 30 ]; then
            echo "BAD INPUT: ${MONTH^} does not have $DAY days."    
            exit 1
        fi
        ;;
    feb)
        check_year $YEAR
        YEAR_RETURN=$?
        if [ $YEAR_RETURN == 1 ];
        then
            if [ $DAY -gt 29 ];
            then 
                echo "BAD INPUT: ${MONTH^} $YEAR does not have $DAY days."
                exit 1
            fi
        else
            if [ $DAY -gt 28 ];
            then
                echo "BAD INPUT: ${MONTH^} $YEAR does not have $DAY days: not a leap year."
                exit 1
            fi
        fi
        ;;
    *)
        echo "BAD INPUT: $1 is not a month that exists."
        exit 1
        ;;
esac
echo "EXISTS! ${MONTH^} $DAY $YEAR is someone's birthday!"