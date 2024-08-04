msg = 'test message'
s = 'this is a <!--message-->'
s = string.gsub(s, '%<%!%-%-message%-%-%>', msg,1)
print(s)