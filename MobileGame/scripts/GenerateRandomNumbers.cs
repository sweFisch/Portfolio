using System.Collections.Generic;
using UnityEngine;

// Just a little benchmark
public class GenerateRandomnumbers : MonoBehaviour
{
    [SerializeField] int generateNumbersPerFrame = 100;
    [SerializeField] bool useUnity = true;

    System.Random random = new System.Random();

    List<int> numbers = new List<int>();

    int _randomValue;

    private void Start()
    {
        random = new System.Random(1337);

        var watch = System.Diagnostics.Stopwatch.StartNew();
        
        for (int i = 0; i < generateNumbersPerFrame; i++) 
        {
            _randomValue = random.Next();
            numbers.Add(_randomValue);
            //print(_randomValue);
        }

        watch.Stop();
        var elapsedMs = watch.ElapsedMilliseconds;
        print(elapsedMs);

        print("Random number count : " + numbers.Count);
        print(numbers[1000]);
    }


    void Update()
    {
        //if (useUnity)
        //{
        //    for (int i = 0; i < generateNumbersPerFrame; i++)
        //    {
        //        _randomValue = UnityEngine.Random.value;
        //    }
        //}
        //else
        //{
        //    for (int i = 0; i < generateNumbersPerFrame; i++)
        //    {
        //        _randomValue = random.Next();
        //    }
        //}
    }
}
