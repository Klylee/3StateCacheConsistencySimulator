enum State
{
    M,
    S,
    I
};

static char stateInChar[3] = {'M', 'S', 'I'};

char getStateChar(State s)
{
    return stateInChar[s];
}
