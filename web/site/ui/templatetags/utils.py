from django import template
register = template.Library()

@register.filter(name='avgcalc')
def avgcalc(num_array):

    if type(num_array) != list:
        return num_array
    num_set = []

    for x in num_array:
        num_set.append(x)

    intTotal = 0
    intCount = 0
    intLenMyList = len(num_array)

    while(intCount <  intLenMyList):
        intTotal += num_set[intCount]
        intCount += 1

    return intTotal/intLenMyList