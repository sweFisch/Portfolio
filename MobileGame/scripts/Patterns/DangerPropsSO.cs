using UnityEngine;

[CreateAssetMenu(fileName ="DangerProps Collection", menuName = "LevelGen/DangerPropsCollectionSO")]
public class DangerPropsSO : ScriptableObject
{
    // This class if for organising Danger props and making it easier to spawn and change looks of different levels if we want to.

    // simple small props
    public GameObject[] Low;
    public GameObject[] LowDuck;
    public GameObject[] High;
    public GameObject[] HighDuck;

    // Plattforms and moving danger
    public GameObject[] LowPlattform;
    public GameObject[] HighPlattform;
    public GameObject[] LowMoving;
    public GameObject[] HighMoving;

    public GameObject GetProp(EDangerProp type)
    {
        int index;

        switch (type)
        {
            case EDangerProp.Low:
                index = Random.Range(0, Low.Length);
                return Low[index];
                
            case EDangerProp.LowDuck:
                index = Random.Range(0, LowDuck.Length);
                return LowDuck[index];

            case EDangerProp.LowPlattform:
                index = Random.Range(0, LowPlattform.Length);
                return LowPlattform[index];

            case EDangerProp.HighPlattform:
                index = Random.Range(0, HighPlattform.Length);
                return HighPlattform[index];

            case EDangerProp.HighDuck:
                index = Random.Range(0, HighDuck.Length);
                return HighDuck[index];

            case EDangerProp.High:
                index = Random.Range(0, High.Length);
                return High[index];

            case EDangerProp.HighMoving:
                index = Random.Range(0, HighMoving.Length);
                return HighMoving[index];
            case EDangerProp.LowMoving:
                index = Random.Range(0, LowMoving.Length);
                return LowMoving[index];

        }

        return null;
    }

    public GameObject GetRandomSmallLowDanger()
    {
        int indexMax = (Low.Length + LowDuck.Length);
        int index = UnityEngine.Random.Range(0, indexMax);

        if (index < Low.Length)
        {
            return Low[index];
        }
        else
        {
            if (LowDuck.Length < 1) { return null; }

            return LowDuck[index - Low.Length];
        }
    }

    public GameObject GetRandomSmallHighDanger()
    {
        int indexMax = (High.Length + HighDuck.Length);
        int index = UnityEngine.Random.Range(0, indexMax);

        if (index < High.Length)
        {
            return High[index];
        }
        else
        {
            if (HighDuck.Length < 1) { return null; }

            return HighDuck[index - High.Length];
        }
    }
}
