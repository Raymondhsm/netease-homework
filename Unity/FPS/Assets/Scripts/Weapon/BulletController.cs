using UnityEngine;

public class BulletController : MonoBehaviour
{
	public float lifeTime = 5f;
	public float bulletSpeed = 10f;
	[SerializeField]
	private int _bulletDamage = 5;

	private Vector3 direction;
	private float time;
	private GameObject _canvas;
	private Vector3 _staticHitPos;
	private int _eid;
	private int _owner;

	private GameObject _bulletHole;
	private GameObject _bulletDamageObj;

    // Start is called before the first frame update
    void Start()
    {
		time = 0f;
		_eid = 0;
		direction = transform.forward;
		_canvas = GameObject.Find("Canvas");

		_bulletHole = (GameObject)Resources.Load("Prefabs/BulletHole");
		_bulletDamageObj = (GameObject)Resources.Load("Prefabs/BulletDamage");
    }

    // Update is called once per frame
    void Update()
    {
		time += Time.deltaTime;
		if (time > lifeTime) Destroy(gameObject);
		
    }

	private void OnTriggerEnter(Collider other)
	{
		var obj = other.gameObject;
		if(obj.CompareTag("Enemy") && _owner == 0)
		{
			if(obj.GetComponent<BehaviorController>().Invincible)
				return;
			GameObject go = Instantiate(_bulletDamageObj);
			BulletDamageController bdc = go.GetComponent<BulletDamageController>();
			bdc.Parent = _canvas;
			bdc.Position = transform.position;
			Destroy(go, 5);
			Destroy(gameObject);
		}
		else if(obj.CompareTag("Terrain"))
		{
			GameObject bulletHole = Instantiate(_bulletHole);
			bulletHole.transform.position = _staticHitPos;
			bulletHole.transform.LookAt(transform);
			bulletHole.transform.Translate(Vector3.back * 0.01f);
			Destroy(bulletHole, 5);
			Destroy(gameObject);
		}
		else if(obj.CompareTag("Player") && _owner == 1)
		{
			Destroy(gameObject);
		}
	}

	public void AddForce(Vector3 dir)
	{
		this.direction = dir;
		transform.GetComponent<Rigidbody>().AddForce(dir * bulletSpeed, ForceMode.Impulse);
	}

	public int BulletDamage
	{
		set { _bulletDamage = value; }
		get { return _bulletDamage; }
	}

	public Vector3 Direction
	{
		set { direction = value; }
	}

	public Vector3 StaticHitPos
	{
		set { _staticHitPos = value; }
	}

	public int eid
	{
		set { _eid = value; }
		get { return _eid; }
	}

	public int owner
	{
		set { _owner = value; }
		get { return _owner; }
	}
}
