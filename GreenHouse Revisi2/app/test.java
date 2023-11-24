import com.example.greenhouse.R;

public class test
{
    public void run()
    {

        int suhu = getSuhu();

        // Set text with the new temperature
        suhuTextView.setText("Suhu: " + suhu + "°C");

        // Adjust text color based on temperature
        if (suhu < 10) {
            suhuTextView.setTextColor(ContextCompat.getColor(getApplicationContext(), R.color.blue));
        } else if (suhu < 20) {
            suhuTextView.setTextColor(ContextCompat.getColor(getApplicationContext(), R.color.green));
        } else {
            suhuTextView.setTextColor(ContextCompat.getColor(getApplicationContext(), R.color.red));
        }

        handler.postDelayed(this, 10000);
    }
        handler.post(updateRunnable);
}
