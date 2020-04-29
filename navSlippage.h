// if analog input pin 0 is unconnected, random analog
// noise will cause the call to randomSeed() to generate
// different seed numbers each time the sketch runs.
// randomSeed() will then shuffle the random function.
void seedSlippage()
{
    randomSeed(analogRead(0));
}

// Function to add a random splippage angle to an intended
// robot turn angle. The slippage is assumed to be uniformly
// distributed between -20 and +20 degrees, in increments
// of 5 degrees.
double navSlippage(double intendedAngle)
{
    double actualAngle,slippage;

    slippage = ((double) random(-4,5)) * 5.0;
    return actualAngle = intendedAngle + slippage;
}
