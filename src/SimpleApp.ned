package simplewireless;

simple SimpleApp {
    parameters:
    	@display("i=block/app2");
    	//Volatile per ottenere una trasmissione aperiodica (Così il periodo cambia ogni volta)
    	volatile double period @unit(s) = default(0s); 
    	int payload @unit(B) = default(10B);
    	string destAddress = default("");
    gates:
        input lowerLayerIn;
        output lowerLayerOut;
}
